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
}


void ServerGame::run() {
    LOG("[Server] Starting game loop...");
    load_players(ASSETS_PATH "/Config_assets/Players/players.json");
    load_level(ASSETS_PATH "/Config_assets/Levels/level.json");
    initialize_player_positions();
    index_existing_entities();

    const int tick_ms = 16;
    float dt = 0.016f;

    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();

        // --- Handle players only ---
        process_pending_messages();
        process_player_inputs(dt);
        apply_gravity(dt);
        check_death_zone();
        check_win_condition();

        // --- Broadcast only players ---
        broadcast_states_to_clients();
        broadcast_player_health();
        broadcast_global_score();
        broadcast_individual_scores();

        // --- Maintain tick rate ---
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
    _platforms.clear();

    auto &types = registry_server.get_components<component::type>();

    for (std::size_t i = 0; i < types.size(); ++i) {
        if (!types[i])
            continue;

        ecs::entity_t entity = registry_server.entity_from_index(i);
        switch (types[i]->value) {
            case component::entity_type::OBSTACLE:
                _obstacles.push_back(entity);
                break;
            case component::entity_type::PLATFORM:
                _platforms.push_back(entity);
                break;
            default:
                break;
        }
    }
    
    LOG_DEBUG("[Server] Indexed entities: Obstacles=" << _obstacles.size() 
              << ", Platforms=" << _platforms.size());
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

        if (state.left != state.right)
            inputX = state.left ? -1.f : 1.f;

        if (inputX != 0.f) {
            auto &pos = kv.second;
            float distance = speedPerSecond * dt;
            float newX = pos.first + inputX * distance;

            if (!is_position_blocked(newX, pos.second, playerWidth, playerHeight, _obstacles) &&
                !is_position_blocked_platform(newX, pos.second, playerWidth, playerHeight, _platforms, false, state.down))
                pos.first = newX;
        }
    }
}

void ServerGame::apply_gravity(float dt) {
    constexpr float gravity = 600.f;
    constexpr float jumpForce = -400.f;
    constexpr float playerWidth = 30.f;
    constexpr float playerHeight = 30.f;
    constexpr int maxJumps = 2; // Player can jump 2 additional times after leaving ground

    for (auto &kv : playerPositions) {
        uint32_t clientId = kv.first;
        if (deadPlayers.find(clientId) != deadPlayers.end())
            continue;

        auto &pos = kv.second;
        auto &velocity = playerVelocities[clientId];
        auto &state = playerInputStates[clientId];
        auto &jumpCount = playerJumpCount[clientId];

        bool onGround = is_on_ground(pos.first, pos.second, playerWidth, playerHeight);

        // Reset jump count when on ground
        if (onGround) {
            jumpCount = 0;
        }

        // Handle jumping with counter
        if (state.up) {
            if (onGround || jumpCount < maxJumps) {
                velocity = jumpForce;
                if (!onGround) {
                    jumpCount++;
                    LOG_DEBUG("[Server] Player " << clientId << " performed air jump " << jumpCount << "/" << maxJumps);
                }
                state.up = false;
            }
        }

        velocity += gravity * dt;

        float newY = pos.second + velocity * dt;

        if (!is_position_blocked(pos.first, newY, playerWidth, playerHeight, _obstacles)) {
            bool movingDown = (velocity > 0.f);
            
            // If down is pressed, pass through platforms
            if (!is_position_blocked_platform(pos.first, newY, playerWidth, playerHeight, _platforms, movingDown, state.down)) {
                pos.second = newY;
            } else {
                if (movingDown) {
                    pos.second = snap_to_platform_top(pos.first, pos.second, playerWidth, playerHeight);
                    velocity = 0.f;
                }
            }
        } else {
            velocity = 0.f;
        }

        if (onGround && velocity > 0.f) {
            velocity = 0.f;
        }
    }
}

bool ServerGame::is_on_ground(float x, float y, float playerWidth, float playerHeight) {
    constexpr float checkDistance = 2.f;
    float testY = y + checkDistance;
    
    return is_position_blocked(x, testY, playerWidth, playerHeight, _obstacles) ||
           is_position_blocked_platform(x, testY, playerWidth, playerHeight, _platforms, true, false);
}

template <typename Component>
inline Component* get_component_ptr_obstacle(ecs::registry &registry, ecs::entity_t entity) {
    auto &arr = registry.get_components<Component>();
    auto idx = static_cast<std::size_t>(entity);
    if (idx >= arr.size() || !arr[idx].has_value()) {
        return nullptr;
    }
    return &arr[idx].value();
}

float ServerGame::snap_to_platform_top(float x, float y, float playerWidth, float playerHeight) {
    constexpr float epsilon = 5.f;
    
    for (auto entity : _platforms) {
        auto pos = get_component_ptr_obstacle<component::position>(registry_server, entity);
        auto box = get_component_ptr_obstacle<component::collision_box>(registry_server, entity);

        if (!pos || !box)
            continue;

        float pLeft = x - playerWidth / 2.f;
        float pRight = x + playerWidth / 2.f;
        float oLeft = pos->x - box->width / 2.f;
        float oRight = pos->x + box->width / 2.f;

        if (pRight > oLeft && pLeft < oRight) {
            float platformTop = pos->y - box->height / 2.f;
            if (std::abs(y + playerHeight / 2.f - platformTop) < epsilon) {
                return platformTop - playerHeight / 2.f;
            }
        }
    }
    return y;
}

void ServerGame::check_death_zone() {
    constexpr float deathX = 1080.f; // Changed from Y to X
    
    for (auto &kv : playerPositions) {
        uint32_t clientId = kv.first;
        if (deadPlayers.find(clientId) != deadPlayers.end())
            continue;

        auto &pos = kv.second;
        if (pos.first > deathX) { // Check X instead of Y
            LOG_INFO("[Server] Player " << clientId << " went past X=" << deathX << " and died");
            deadPlayers.insert(clientId);
            broadcast_player_death(clientId);
            
            auto& healths = registry_server.get_components<component::health>();
            auto& clientIds = registry_server.get_components<component::client_id>();
            for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
                if (clientIds[i] && clientIds[i]->id == clientId && healths[i]) {
                    healths[i]->current = 0;
                    break;
                }
            }
        }
    }
}

void ServerGame::check_win_condition() {
    int alivePlayers = 0;
    uint32_t lastAlivePlayer = 0;

    for (const auto& kv : connexion.getClients()) {
        uint32_t clientId = kv.second;
        if (deadPlayers.find(clientId) == deadPlayers.end()) {
            alivePlayers++;
            lastAlivePlayer = clientId;
        }
    }

    // Win condition: only 1 player remaining and at least 2 players total
    if (alivePlayers == 1 && connexion.getClients().size() >= 2) {
        LOG_INFO("[Server] Player " << lastAlivePlayer << " is the last one alive and WINS!");
        broadcast_game_winner(lastAlivePlayer);
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
        float z = 0.f;
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

void ServerGame::broadcast_game_winner(uint32_t winnerId) {
    // You'll need to define a GameWinnerMessage in your protocol
    // For now, logging it
    LOG_INFO("[Server] Broadcasting winner: Player " << winnerId);
    // TODO: Implement actual broadcast message for winner
}

void ServerGame::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}