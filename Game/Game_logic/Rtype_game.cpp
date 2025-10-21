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


void ServerGame::run() {
    LOG("[Server] Starting game loop...");
    load_players(ASSETS_PATH "/Config_assets/Players/players.json");
    load_level(ASSETS_PATH "/Config_assets/Levels/level_01.json");
    initialize_player_positions();
    index_existing_entities();

    const int tick_ms = 16;
    float dt = 0.016f;

    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();

        process_pending_messages();
        process_player_inputs(dt);

        update_projectiles_server_only(dt);
        update_enemies(dt);
        update_enemy_projectiles_server_only(dt);

        check_enemy_projectile_player_collisions();
        check_projectile_collisions();
        check_projectile_enemy_collisions();
        check_player_enemy_collisions();

        broadcast_projectile_positions();
        broadcast_enemy_positions();
        broadcast_enemy_projectile_positions();

        broadcast_states_to_clients();
        broadcast_player_health();
        broadcast_global_score();
        broadcast_individual_scores();

        sleep_to_maintain_tick(tick_start, tick_ms);
    }
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

    LOG_INFO("[Server] Indexed entities:");
    LOG_DEBUG("  - Enemies: " << _enemies.size());
    LOG_DEBUG("  - Obstacles: " << _obstacles.size());
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
                        LOG_DEBUG("[Server][Input] queued event from client " << id
                                  << " code=" << inputCodeToString(code)
                                  << " pressed=" << pressed
                                  << " buffered=" << playerInputBuffers[id].size());
                        break;
                    default:
                        LOG_DEBUG("[Server][Input] ignored unknown code "
                                  << static_cast<int>(msg->inputCode)
                                  << " from client " << id);
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
                    float projZ = 0.f;  // Ajout de Z
                    float projVelX = 400.f;
                    float projVelY = 0.f;
                    float projVelZ = 0.f;  // Ajout de Z
                    
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
        default:
            break;
    }
}

void ServerGame::process_pending_messages() {
    int messagesProcessed = 0;
    const int maxMessagesPerTick = 5;
    while (messagesProcessed < maxMessagesPerTick) {
        connexion.asyncReceive([this](const asio::error_code& ec, std::vector<uint8_t> data, asio::ip::udp::endpoint endpoint) {
            if (ec)
                return;
            handle_client_message(data, endpoint);
        });
        messagesProcessed++;
        std::this_thread::sleep_for(std::chrono::microseconds(100));
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

        LOG_DEBUG("[Server][Input] applied movement for client " << clientId
                  << " dir=(" << inputX << "," << inputY << ")"
                  << " newPos=(" << pos.first << "," << pos.second << ")");
    }
}

void ServerGame::broadcast_states_to_clients() {
    for (const auto& kv : connexion.getClients()) {
        uint32_t id = kv.second;
        if (deadPlayers.find(id) != deadPlayers.end())
            continue;
        auto it = playerPositions.find(id);
        if (it == playerPositions.end()) continue;
        
        StateUpdateMessage m;
        m.type = MessageType::StateUpdate;
        m.clientId = htonl(id);
        
        uint32_t xbits, ybits, zbits;
        float z = 0.f;  // Valeur par défaut pour Z
        std::memcpy(&xbits, &it->second.first, sizeof(float));
        std::memcpy(&ybits, &it->second.second, sizeof(float));
        std::memcpy(&zbits, &z, sizeof(float));
        
        m.pos.xBits = htonl(xbits);
        m.pos.yBits = htonl(ybits);
        m.pos.zBits = htonl(zbits);
        
        connexion.broadcast(&m, sizeof(m));
    }
}

void ServerGame::broadcast_obstacle_despawn(uint32_t obstacleId) {
    ObstacleDespawnMessage m;
    m.type = MessageType::ObstacleDespawn;
    m.obstacleId = htonl(obstacleId);
    connexion.broadcast(&m, sizeof(m));
}

void ServerGame::broadcast_player_health() {
    auto& healths = registry_server.get_components<component::health>();
    auto& clientIds = registry_server.get_components<component::client_id>();
    
    for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
        if (healths[i] && clientIds[i]) {
            PlayerHealthMessage msg;
            msg.type = MessageType::PlayerHealth;
            msg.clientId = htonl(clientIds[i]->id);
            msg.currentHealth = htons(static_cast<int16_t>(healths[i]->current));
            msg.maxHealth = htons(static_cast<int16_t>(healths[i]->max));
            
            connexion.broadcast(&msg, sizeof(msg));
        }
    }
}

void ServerGame::broadcast_global_score() {
    GlobalScoreMessage msg;
    msg.type = MessageType::GlobalScore;
    msg.totalScore = htonl(totalScore);
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_individual_scores() {
    for (const auto& kv : playerIndividualScores) {
        uint32_t clientId = kv.first;
        int score = kv.second;
        
        IndividualScoreMessage msg;
        msg.type = MessageType::IndividualScore;
        msg.clientId = htonl(clientId);
        msg.score = htonl(score);
        
        connexion.broadcast(&msg, sizeof(msg));
    }
}

void ServerGame::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}