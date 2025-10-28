/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/

#include "Rtype_game.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <cctype>

#define RESET_COLOR   "\033[0m"
#define RED_COLOR     "\033[31m"
#define GREEN_COLOR   "\033[32m"
#define YELLOW_COLOR  "\033[33m"
#define BLUE_COLOR    "\033[34m"
#define LOG_ERROR(msg)   std::cerr << RED_COLOR << "[ERROR] " << msg << RESET_COLOR << std::endl
#define LOG_INFO(msg)    std::cout << GREEN_COLOR << "[INFO] " << msg << RESET_COLOR << std::endl
#define LOG_DEBUG(msg)   std::cout << YELLOW_COLOR << "[DEBUG] " << msg << RESET_COLOR << std::endl
#define LOG(msg)         std::cout << BLUE_COLOR << msg << RESET_COLOR << std::endl


static nlohmann::json load_json_from_file(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open JSON file: " + path);
    }
    nlohmann::json data;
    file >> data;
    return data;
}

ServerGame::ServerGame(Connexion &conn) : connexion(conn), registry_server() {
    _roomId = -1;
    registry_server.register_component<component::position>();
    registry_server.register_component<component::previous_position>();
    registry_server.register_component<component::velocity>();
    registry_server.register_component<component::rotation>();
    registry_server.register_component<component::scale>();
    registry_server.register_component<component::dynamic_position>();
    registry_server.register_component<component::drawable>();
    registry_server.register_component<component::sprite>();
    registry_server.register_component<component::model3D>();
    registry_server.register_component<component::audio>();
    registry_server.register_component<component::text>();
    registry_server.register_component<component::font>();
    registry_server.register_component<component::clickable>();
    registry_server.register_component<component::hoverable>();
    registry_server.register_component<component::controllable>();
    registry_server.register_component<component::health>();
    registry_server.register_component<component::damage>();
    registry_server.register_component<component::collision_box>();
    registry_server.register_component<component::hitbox_link>();
    registry_server.register_component<component::type>();
    registry_server.register_component<component::client_id>();
    registry_server.register_component<component::pattern_element>();
}

void ServerGame::setInitialPlayerSkins(const std::unordered_map<uint32_t, std::string> &skins) {
    _playerSkins = skins;
}


void ServerGame::run(int roomId) {
    LOG("[Server] Starting game loop...");
    _roomId = roomId;
    load_players(ASSETS_PATH "/Config_assets/Players/players.json");
    load_level(ASSETS_PATH "/Config_assets/Levels/level_01.json");
    initialize_player_positions();
    index_existing_entities();

    if (!_playerSkins.empty()) {
        const std::string assetsPlayerDir = std::string(ASSETS_PATH) + "/sprites/player/";
        auto &sprites = registry_server.get_components<component::sprite>();
        auto &clientIds = registry_server.get_components<component::client_id>();
        for (std::size_t i = 0; i < sprites.size() && i < clientIds.size(); ++i) {
            if (!sprites[i] || !clientIds[i]) continue;
            auto it = _playerSkins.find(clientIds[i]->id);
            if (it != _playerSkins.end() && !it->second.empty()) {
                sprites[i]->image_path = assetsPlayerDir + it->second;
            }
        }
        for (const auto &entry : _playerSkins) {
            broadcast_player_skin(entry.first, entry.second);
        }
    }

    const int tick_ms = 16;
    float dt = 0.016f;

    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();

        process_pending_messages();
        process_player_inputs(dt);

        update_projectiles_server_only(dt);
        update_enemies(dt);
        update_obstacles(dt);
        update_enemy_projectiles_server_only(dt);

        check_enemy_projectile_player_collisions();
        check_projectile_collisions();
        check_projectile_enemy_collisions();
        check_player_enemy_collisions();

        broadcast_projectile_positions();
        broadcast_obstacle_positions();
        broadcast_enemy_positions();
        broadcast_enemy_projectile_positions();

        broadcast_states_to_clients();
        broadcast_player_health();
        broadcast_global_score();
        broadcast_individual_scores();

        if (levelTransitionPending) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - levelTransitionTime
            ).count();
            
            if (elapsed >= LEVEL_TRANSITION_DELAY) {
                load_next_level();
                levelTransitionPending = false;
            }
        }
        sleep_to_maintain_tick(tick_start, tick_ms);
    }
}

void ServerGame::enqueuePacket(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) {
    std::lock_guard<std::mutex> lock(packetMutex);
    pendingPackets.push(PendingPacket{data, from});
}

void ServerGame::setInitialClients(const std::vector<uint32_t> &clients) {
    std::lock_guard<std::mutex> lock(initialClientsMutex);
    initialClients = clients;
}

bool ServerGame::check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                        float left2, float right2, float top2, float bottom2) {
    return right1 > left2 && left1 < right2 && bottom1 > top2 && top1 < bottom2;
}

void ServerGame::broadcast_full_registry_to(uint32_t clientId) {
    nlohmann::json root;
    root["type"] = "FullRegistry";
    root["entities"] = nlohmann::json::array();

    for (auto entity : registry_server.alive_entities()) {
        auto j = game::serializer::serialize_entity(registry_server, entity);
        if (!j.empty()) {
            root["entities"].push_back(j);
        }
    }
    connexion.sendJsonToClient(clientId, root);
    LOG_DEBUG("[Server] Sent full registry to client " << clientId 
              << " with " << root["entities"].size() << " entities");
}

void ServerGame::load_players(const std::string &path) {
    try {
        auto json = load_json_from_file(path);
        game::storage::store_players(registry_server, json);
        auto &positions = registry_server.get_components<component::position>();
        auto &clientIds = registry_server.get_components<component::client_id>();
        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].has_value() && clientIds[i].has_value()) {
                auto &pos = positions[i].value();
                auto &cid = clientIds[i].value();
                ecs::entity_t entity = registry_server.entity_from_index(i);
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("load_players failed: " << e.what());
    }
}

void ServerGame::load_level(const std::string &path) {
    try {
        auto json = load_json_from_file(path);
        game::storage::store_level_entities(registry_server, json);
        auto &positions = registry_server.get_components<component::position>();
        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].has_value()) {
                auto &pos = positions[i].value();
                ecs::entity_t entity = registry_server.entity_from_index(i);
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("load_level failed: " << e.what());
    }
}

void ServerGame::index_existing_entities() {
    _obstacles.clear();
    _enemies.clear();

    auto &types = registry_server.get_components<component::type>();

    for (std::size_t i = 0; i < types.size(); ++i) {
        if (!types[i])
            continue;

        ecs::entity_t entity = registry_server.entity_from_index(i);
        switch (types[i]->value) {
            case component::entity_type::ENEMY:
                _enemies.push_back(entity);
                break;
            case component::entity_type::OBSTACLE:
                _obstacles.push_back(entity);
                break;
            default:
                break;
        }
    }

    auto &positions = registry_server.get_components<component::position>();
    auto &velocities = registry_server.get_components<component::velocity>();
    auto &drawables = registry_server.get_components<component::drawable>();
    
    for (auto enemy : _enemies) {
        uint32_t id = static_cast<uint32_t>(enemy);
        if (id >= positions.size() || !positions[id]) continue;
        
        float x = positions[id]->x;
        float y = positions[id]->y;
        float z = positions[id]->z;
        float vx = 0, vy = 0, vz = 0;
        float w = 0.0f, h = 0.0f;
        
        if (id < velocities.size() && velocities[id]) {
            vx = velocities[id]->vx;
            vy = velocities[id]->vy;
            vz = velocities[id]->vz;
        }
        
        if (id < drawables.size() && drawables[id]) {
            w = drawables[id]->width;
            h = drawables[id]->height;
        }
        
        broadcast_enemy_spawn(id, x, y, z, vx, vy, vz, w, h);
    }

    for (auto obstacle : _obstacles) {
        uint32_t id = static_cast<uint32_t>(obstacle);
        if (id >= positions.size() || !positions[id]) continue;
        
        float x = positions[id]->x;
        float y = positions[id]->y;
        float z = positions[id]->z;
        float w = 0.0f, h = 0.0f, d = 50.0f;
        float vx = 0, vy = 0, vz = 0;
        
        if (id < drawables.size() && drawables[id]) {
            w = drawables[id]->width;
            h = drawables[id]->height;
            d = drawables[id]->depth;
        }

        if (id < velocities.size() && velocities[id]) {
            vx = velocities[id]->vx;
            vy = velocities[id]->vy;
            vz = velocities[id]->vz;
        }
        
        broadcast_obstacle_spawn(id, x, y, z, w, h, d, vx, vy, vz);
    }

    LOG_INFO("[Server] Indexed entities:");
    LOG_DEBUG("  - Enemies: " << _enemies.size());
    LOG_DEBUG("  - Obstacles: " << _obstacles.size());
}

void ServerGame::load_next_level() {
    LOG_INFO("[Server] Loading next level...");
    currentLevel++;
    clear_level_entities();
    std::string levelPath = ASSETS_PATH "/Config_assets/Levels/level_0" 
                          + std::to_string(currentLevel) + ".json";
    try {
        load_level(levelPath);
        index_existing_entities();
        for (const auto& kv : connexion.getClients()) {
            broadcast_full_registry_to(kv.second);
        }
        LOG_INFO("[Server] Level " << currentLevel << " loaded successfully!");
    } catch (const std::exception &e) {
        LOG_ERROR("[Server] Failed to load level " << currentLevel << ": " << e.what());
        LOG_INFO("[Server] Game completed! No more levels.");
    }
}

void ServerGame::clear_level_entities() {
    auto &types = registry_server.get_components<component::type>();
    std::vector<ecs::entity_t> entitiesToKill;    
    for (std::size_t i = 0; i < types.size(); ++i) {
        if (!types[i]) continue;
        
        ecs::entity_t entity = registry_server.entity_from_index(i);
        
        switch (types[i]->value) {
            case component::entity_type::ENEMY:
            case component::entity_type::OBSTACLE:
            case component::entity_type::PROJECTILE:
            case component::entity_type::POWERUP:
            case component::entity_type::BACKGROUND:
                entitiesToKill.push_back(entity);
                break;
            default:
                break;
        }
    }    
    for (auto entity : entitiesToKill) {
        registry_server.kill_entity(entity);
    }
    _enemies.clear();
    _obstacles.clear();
    projectiles.clear();
    enemyProjectiles.clear();
    LOG_DEBUG("[Server] Cleared " << entitiesToKill.size() << " level entities");
}

void ServerGame::handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from) {
    if (data.size() < sizeof(MessageType))
        return;
    MessageType type = *reinterpret_cast<const MessageType*>(data.data());
    switch(type) {
        case MessageType::ClientInput: {
            if (data.size() >= sizeof(ClientInputMessage)) {
                const ClientInputMessage* msg = reinterpret_cast<const ClientInputMessage*>(data.data());
                uint32_t id = ntohl(msg->clientId);
                if (deadPlayers.find(id) != deadPlayers.end())
                    break;
                InputCode code = static_cast<InputCode>(msg->inputCode);
                bool pressed = msg->isPressed != 0;
                switch (code) {
                    case InputCode::Up:
                    case InputCode::Down:
                    case InputCode::Left:
                    case InputCode::Right:
                        playerInputBuffers[id].push_back({code, pressed});
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        case MessageType::SceneState: {
            if (data.size() >= sizeof(SceneStateMessage)) {
                const SceneStateMessage* msg = reinterpret_cast<const SceneStateMessage*>(data.data());
                uint32_t id = ntohl(msg->clientId);
                SceneState scene = static_cast<SceneState>(ntohl(msg->scene));
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    clientScenes[id] = scene;
                }
                LOG_DEBUG("[Server] Client " << id << " switched to scene = " << static_cast<int>(scene));
                if (scene == SceneState::GAME) {
                    broadcast_full_registry_to(id);
                    send_player_skins_to(id);
                }
            }
            break;
        }
        case MessageType::PlayerSkinUpdate: {
            if (data.size() >= sizeof(PlayerSkinMessage)) {
                const PlayerSkinMessage *msg = reinterpret_cast<const PlayerSkinMessage*>(data.data());
                uint32_t clientId = ntohl(msg->clientId);
                std::string filename(msg->skinFilename);
                if (filename.empty()) {
                    _playerSkins.erase(clientId);
                } else {
                    _playerSkins[clientId] = filename;
                }
                broadcast_player_skin(clientId, filename);

                const std::string assetsPlayerDir = std::string(ASSETS_PATH) + "/sprites/player/";
                auto &sprites = registry_server.get_components<component::sprite>();
                auto &clientIds = registry_server.get_components<component::client_id>();
                for (std::size_t i = 0; i < sprites.size() && i < clientIds.size(); ++i) {
                    if (clientIds[i] && clientIds[i]->id == clientId && sprites[i]) {
                        if (!filename.empty()) {
                            sprites[i]->image_path = assetsPlayerDir + filename;
                        } else {
                            sprites[i]->image_path = assetsPlayerDir + "r-typesheet42.png";
                        }
                        break;
                    }
                }
            }
            break;
        }
        case MessageType::ClientShoot: {
            if (data.size() >= sizeof(ClientShootMessage)) {
                const ClientShootMessage* msg = reinterpret_cast<const ClientShootMessage*>(data.data());
                uint32_t clientId = ntohl(msg->clientId);
                if (deadPlayers.find(clientId) != deadPlayers.end())
                    break;

                auto it = playerPositions.find(clientId);
                if (it != playerPositions.end()) {
                    float playerX = it->second.first;
                    float playerY = it->second.second;

                    uint32_t projId = nextProjectileId++;
                    float projX = playerX + 20.f;
                    float projY = playerY;
                    float projZ = 0.f;
                    float projVelX = 400.f;
                    float projVelY = 0.f;
                    float projVelZ = 0.f;
                    
                    projectiles[projId] = std::make_tuple(projX, projY, projZ, projVelX, projVelY, projVelZ, clientId);

                    broadcast_projectile_spawn(projId, clientId, projX, projY, projZ, projVelX, projVelY, projVelZ);

                    LOG_DEBUG("[Server] Client " << clientId << " shot projectile " << projId);
                }
            }
            break;
        }
        case MessageType::InitialHealth: {
            if (data.size() >= sizeof(InitialHealthMessage)) {
                const InitialHealthMessage* msg = reinterpret_cast<const InitialHealthMessage*>(data.data());
                uint32_t clientId = ntohl(msg->clientId);
                int16_t initialHealth = ntohs(msg->initialHealth);
                auto& healths = registry_server.get_components<component::health>();
                auto& clientIds = registry_server.get_components<component::client_id>();
                for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
                    if (clientIds[i] && clientIds[i]->id == clientId) {
                        if (healths[i]) {
                            healths[i]->current = initialHealth;
                            healths[i]->max = initialHealth;
                            LOG_DEBUG("[Server] Set initial health of client " << clientId << " to " << initialHealth);
                        }
                        break;
                    }
                }
            }
            break;
        }
        case MessageType::ChatMessage: {
            if (data.size() >= sizeof(ChatMessagePacket)) {
                const ChatMessagePacket *msg = reinterpret_cast<const ChatMessagePacket *>(data.data());
                uint32_t clientId = ntohl(msg->senderId);

                auto messageEnd = std::find(std::begin(msg->message), std::end(msg->message), '\0');
                std::string text(msg->message, static_cast<std::size_t>(messageEnd - std::begin(msg->message)));

                auto trimWhitespace = [](std::string &str) {
                    auto isSpace = [](unsigned char ch) { return std::isspace(ch) != 0; };
                    str.erase(str.begin(), std::find_if_not(str.begin(), str.end(), isSpace));
                    str.erase(std::find_if_not(str.rbegin(), str.rend(), isSpace).base(), str.end());
                };

                trimWhitespace(text);
                for (char &ch : text) {
                    if (ch == '\n' || ch == '\r')
                        ch = ' ';
                }
                trimWhitespace(text);

                if (text.empty())
                    break;

                std::string senderName = connexion.getClientName(clientId);
                if (senderName.empty()) {
                    auto senderEnd = std::find(std::begin(msg->senderName), std::end(msg->senderName), '\0');
                    senderName.assign(msg->senderName, static_cast<std::size_t>(senderEnd - std::begin(msg->senderName)));
                }
                trimWhitespace(senderName);

                ChatMessagePacket outgoing{};
                outgoing.type = MessageType::ChatMessage;
                outgoing.senderId = htonl(clientId);
                std::memset(outgoing.senderName, 0, sizeof(outgoing.senderName));
                std::memset(outgoing.message, 0, sizeof(outgoing.message));
                if (!senderName.empty())
                    std::strncpy(outgoing.senderName, senderName.c_str(), sizeof(outgoing.senderName) - 1);
                std::strncpy(outgoing.message, text.c_str(), sizeof(outgoing.message) - 1);

                connexion.broadcast(&outgoing, sizeof(outgoing));
                LOG_DEBUG("[Server][Chat] " << (senderName.empty() ? "Player" : senderName) << ": " << text);
            }
            break;
        }
        default:
            break;
    }
}

void ServerGame::process_pending_messages() {
    std::queue<PendingPacket> localQueue;

    {
        std::lock_guard<std::mutex> lock(packetMutex);
        while (!pendingPackets.empty()) {
            localQueue.push(std::move(pendingPackets.front()));
            pendingPackets.pop();
        }
    }

    int processed = 0;
    constexpr int maxMessagesPerTick = 32;
    while (!localQueue.empty() && processed < maxMessagesPerTick) {
        auto packet = std::move(localQueue.front());
        localQueue.pop();
        handle_client_message(packet.data, packet.endpoint);
        processed++;
    }
}

void ServerGame::process_player_inputs(float dt) {
    constexpr float speedPerSecond = 200.f;
    constexpr float playerWidth = 30.f;
    constexpr float playerHeight = 30.f;

    for (auto &kv : playerPositions) {
        uint32_t clientId = kv.first;
        if (deadPlayers.find(clientId) != deadPlayers.end())
            continue;

        auto &buffer = playerInputBuffers[clientId];
        auto &state = playerInputStates[clientId];

        for (const auto &event : buffer) {
            switch (event.code) {
                case InputCode::Up:
                    state.up = event.pressed;
                    break;
                case InputCode::Down:
                    state.down = event.pressed;
                    break;
                case InputCode::Left:
                    state.left = event.pressed;
                    break;
                case InputCode::Right:
                    state.right = event.pressed;
                    break;
                default:
                    break;
            }
        }
        buffer.clear();

        float inputX = 0.f;
        float inputY = 0.f;

        if (state.left != state.right)
            inputX = state.left ? -1.f : 1.f;
        if (state.up != state.down)
            inputY = state.up ? -1.f : 1.f;

        if (inputX == 0.f && inputY == 0.f)
            continue;

        auto &pos = kv.second;
        float distance = speedPerSecond * dt;
        float newX = pos.first + inputX * distance;
        float newY = pos.second + inputY * distance;

        if (!is_position_blocked(newX, pos.second, playerWidth, playerHeight, _obstacles))
            pos.first = newX;
        if (!is_position_blocked(pos.first, newY, playerWidth, playerHeight, _obstacles))
            pos.second = newY;
    }
}

void ServerGame::broadcast_states_to_clients() {
    auto recipients = collectRoomClients(false);
    if (recipients.empty())
        return;

    for (uint32_t id : recipients) {
        auto it = playerPositions.find(id);
        if (it == playerPositions.end())
            continue;

        StateUpdateMessage m;
        m.type = MessageType::StateUpdate;
        m.clientId = htonl(id);
        
        uint32_t xbits, ybits, zbits;
        float z = 0.f;
        std::memcpy(&xbits, &it->second.first, sizeof(float));
        std::memcpy(&ybits, &it->second.second, sizeof(float));
        std::memcpy(&zbits, &z, sizeof(float));
        
        m.pos.xBits = htonl(xbits);
        m.pos.yBits = htonl(ybits);
        m.pos.zBits = htonl(zbits);
        
        connexion.broadcastToClients(recipients, &m, sizeof(m));
    }
}

void ServerGame::broadcast_obstacle_despawn(uint32_t obstacleId) {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    ObstacleDespawnMessage m;
    m.type = MessageType::ObstacleDespawn;
    m.obstacleId = htonl(obstacleId);
    connexion.broadcastToClients(recipients, &m, sizeof(m));
}

void ServerGame::broadcast_player_health() {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    auto& healths = registry_server.get_components<component::health>();
    auto& clientIds = registry_server.get_components<component::client_id>();
    
    for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
        if (healths[i] && clientIds[i]) {
            PlayerHealthMessage msg;
            msg.type = MessageType::PlayerHealth;
            msg.clientId = htonl(clientIds[i]->id);
            msg.currentHealth = htons(static_cast<int16_t>(healths[i]->current));
            msg.maxHealth = htons(static_cast<int16_t>(healths[i]->max));
            
            connexion.broadcastToClients(recipients, &msg, sizeof(msg));
        }
    }
}

void ServerGame::broadcast_global_score() {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    GlobalScoreMessage msg;
    msg.type = MessageType::GlobalScore;
    msg.totalScore = htonl(totalScore);
    connexion.broadcastToClients(recipients, &msg, sizeof(msg));
}

void ServerGame::broadcast_individual_scores() {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    for (const auto& kv : playerIndividualScores) {
        uint32_t clientId = kv.first;
        int score = kv.second;
        
        IndividualScoreMessage msg;
        msg.type = MessageType::IndividualScore;
        msg.clientId = htonl(clientId);
        msg.score = htonl(score);
        
        connexion.broadcastToClients(recipients, &msg, sizeof(msg));
    }
}

void ServerGame::broadcast_player_skin(uint32_t clientId, const std::string &filename) {
    PlayerSkinMessage msg{};
    msg.type = MessageType::PlayerSkinUpdate;
    msg.clientId = htonl(clientId);
    std::memset(msg.skinFilename, 0, sizeof(msg.skinFilename));
    std::strncpy(msg.skinFilename, filename.c_str(), sizeof(msg.skinFilename) - 1);
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::send_player_skins_to(uint32_t clientId) {
    (void)clientId;
    if (_playerSkins.empty()) {
        return;
    }
    for (const auto &entry : _playerSkins) {
        broadcast_player_skin(entry.first, entry.second);
    }
}

void ServerGame::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}

std::vector<uint32_t> ServerGame::collectRoomClients(bool includeDead) const {
    std::vector<uint32_t> ids;
    ids.reserve(playerPositions.size());
    for (const auto &kv : playerPositions) {
        if (!includeDead && deadPlayers.find(kv.first) != deadPlayers.end())
            continue;
        ids.push_back(kv.first);
    }
    if (ids.empty()) {
        std::lock_guard<std::mutex> lock(initialClientsMutex);
        ids.reserve(initialClients.size());
        for (auto id : initialClients) {
            if (!includeDead && deadPlayers.find(id) != deadPlayers.end())
                continue;
            ids.push_back(id);
        }
    }
    return ids;
}