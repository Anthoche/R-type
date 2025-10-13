/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/
#include "Include/ServerGame.hpp"
#include "../../Engine/Utils/Include/serializer.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <cmath>
// === Couleurs pour les logs ===
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
    registry_server.register_component<component::controllable>();
    registry_server.register_component<component::health>();
    registry_server.register_component<component::type>();
    registry_server.register_component<component::client_id>();
    registry_server.register_component<component::drawable>();
    registry_server.register_component<component::sprite>();
    registry_server.register_component<component::collision_box>();
    registry_server.register_component<component::hitbox_link>();
}

void ServerGame::run() {
    LOG("[Server] Starting game loop...");
    load_players("../Engine/Assets/Config_assets/Players/players.json");
    //load_level("../Engine/Assets/Config_assets/Levels/level_01.json");
    initialize_player_positions();
    initialize_obstacles();
    initialize_enemies();
    
    const int tick_ms = 16;
    float dt = 0.016f;
    
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        
        update_projectiles_server_only(dt);
        update_enemies(dt);
        check_projectile_collisions();
        check_projectile_enemy_collisions();
        check_player_enemy_collisions();
        
        broadcast_projectile_positions();
        broadcast_enemy_positions();
        
        process_pending_messages();
        broadcast_states_to_clients();
        broadcast_player_health();
        broadcast_global_score();
        broadcast_individual_scores();
        
        sleep_to_maintain_tick(tick_start, tick_ms);
    }
}

bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                        float left2, float right2, float top2, float bottom2) {
    return right1 > left2 && left1 < right2 && bottom1 > top2 && top1 < bottom2;
}

void ServerGame::check_projectile_collisions() {
    std::vector<uint32_t> projectilesToRemove;
    std::vector<uint32_t> obstaclesToRemove;
    
    for (const auto& proj : projectiles) {
        uint32_t projId = proj.first;
        float projX = std::get<0>(proj.second);
        float projY = std::get<1>(proj.second);        
        float projLeft = projX - 5.f;
        float projRight = projX + 5.f;
        float projTop = projY - 2.5f;
        float projBottom = projY + 2.5f;
        
        for (const auto& obs : obstacles) {
            uint32_t obsId = obs.first;
            float obsX = std::get<0>(obs.second);
            float obsY = std::get<1>(obs.second);
            float obsW = std::get<2>(obs.second);
            float obsH = std::get<3>(obs.second);
        
            float obsLeft = obsX - obsW * 0.5f;
            float obsRight = obsX + obsW * 0.5f;
            float obsTop = obsY - obsH * 0.5f;
            float obsBottom = obsY + obsH * 0.5f;
            
            if (check_aabb_overlap(projLeft, projRight, projTop, projBottom,
                                  obsLeft, obsRight, obsTop, obsBottom)) {
                projectilesToRemove.push_back(projId);
                break;
            }
        }
    }
    
    for (uint32_t projId : projectilesToRemove) {
        projectiles.erase(projId);
        broadcast_projectile_despawn(projId);
    }
    
    for (uint32_t obsId : obstaclesToRemove) {
        obstacles.erase(obsId);
        broadcast_obstacle_despawn(obsId);
    }
}

void ServerGame::check_player_enemy_collisions() {
    const float PLAYER_WIDTH = 30.f;
    const float PLAYER_HEIGHT = 30.f;
    const float ENEMY_WIDTH = 30.f;
    const float ENEMY_HEIGHT = 30.f;
    const int DAMAGE_PER_HIT = 10;
    const int COOLDOWN_MS = 1000;
    
    std::vector<uint32_t> playersHit;
    auto now = std::chrono::high_resolution_clock::now();
    
    for (const auto& playerKv : playerPositions) {
        uint32_t playerId = playerKv.first;
        
        if (deadPlayers.find(playerId) != deadPlayers.end())
            continue;
        
        float playerX = playerKv.second.first;
        float playerY = playerKv.second.second;
        
        float playerLeft = playerX - PLAYER_WIDTH * 0.5f;
        float playerRight = playerX + PLAYER_WIDTH * 0.5f;
        float playerTop = playerY - PLAYER_HEIGHT * 0.5f;
        float playerBottom = playerY + PLAYER_HEIGHT * 0.5f;
        
        for (const auto& enemy : enemies) {
            float enemyX = std::get<0>(enemy.second);
            float enemyY = std::get<1>(enemy.second);
            
            float enemyLeft = enemyX - ENEMY_WIDTH * 0.5f;
            float enemyRight = enemyX + ENEMY_WIDTH * 0.5f;
            float enemyTop = enemyY - ENEMY_HEIGHT * 0.5f;
            float enemyBottom = enemyY + ENEMY_HEIGHT * 0.5f;
            
            if (check_aabb_overlap(playerLeft, playerRight, playerTop, playerBottom,
                                  enemyLeft, enemyRight, enemyTop, enemyBottom)) {
                
                auto it = playerDamageCooldown.find(playerId);
                bool canTakeDamage = (it == playerDamageCooldown.end());
                
                if (!canTakeDamage) {
                    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second).count();
                    canTakeDamage = (elapsed >= COOLDOWN_MS);
                }
                
                if (canTakeDamage) {
                    playersHit.push_back(playerId);
                    playerDamageCooldown[playerId] = now;
                    LOG_DEBUG("[Server] Player " << playerId << " hit by enemy " << enemy.first 
                            << " - Taking " << DAMAGE_PER_HIT << " damage");
                    LOG_DEBUG("[Server] Cooldown set for player " << playerId);
                }
                break;
            }
        }
    }
    
    for (uint32_t playerId : playersHit) {
        auto& healths = registry_server.get_components<component::health>();
        auto& clientIds = registry_server.get_components<component::client_id>();
        
        for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
            if (clientIds[i] && clientIds[i]->id == playerId && healths[i]) {
                healths[i]->current -= DAMAGE_PER_HIT;
                LOG_INFO("[Server] Player " << playerId << " health: " 
                        << healths[i]->current << "/" << healths[i]->max);
                
                if (healths[i]->current <= 0) {
                    deadPlayers.insert(playerId);
                    broadcast_player_death(playerId);
                    LOG_INFO("[Server] Player " << playerId << " is now dead!");
                }
                break;
            }
        }
    }
}

void ServerGame::check_projectile_enemy_collisions() {
    std::vector<uint32_t> projectilesToRemove;
    std::vector<uint32_t> enemiesToRemove;
    
    const float ENEMY_WIDTH = 30.f;
    const float ENEMY_HEIGHT = 30.f;
    const float PROJ_WIDTH = 10.f;
    const float PROJ_HEIGHT = 5.f;
    
    for (const auto& proj : projectiles) {
        uint32_t projId = proj.first;
        float projX = std::get<0>(proj.second);
        float projY = std::get<1>(proj.second);
        
        float projLeft = projX - PROJ_WIDTH * 0.5f;
        float projRight = projX + PROJ_WIDTH * 0.5f;
        float projTop = projY - PROJ_HEIGHT * 0.5f;
        float projBottom = projY + PROJ_HEIGHT * 0.5f;
        
        for (const auto& enemy : enemies) {
            uint32_t enemyId = enemy.first;
            float enemyX = std::get<0>(enemy.second);
            float enemyY = std::get<1>(enemy.second);
            
            float enemyLeft = enemyX - ENEMY_WIDTH * 0.5f;
            float enemyRight = enemyX + ENEMY_WIDTH * 0.5f;
            float enemyTop = enemyY - ENEMY_HEIGHT * 0.5f;
            float enemyBottom = enemyY + ENEMY_HEIGHT * 0.5f;
            
            if (check_aabb_overlap(projLeft, projRight, projTop, projBottom,
                                  enemyLeft, enemyRight, enemyTop, enemyBottom)) {
                projectilesToRemove.push_back(projId);
                enemiesToRemove.push_back(enemyId);
                
                totalScore += 10;

                uint32_t killerId = std::get<4>(proj.second);
                if (playerIndividualScores.find(killerId) == playerIndividualScores.end()) {
                    playerIndividualScores[killerId] = 0;
                }
                playerIndividualScores[killerId] += 10;
                LOG_DEBUG("[Server] Enemy killed! +10 points. Total: " << totalScore);
                
                LOG_DEBUG("[Server] Projectile " << projId << " hit enemy " << enemyId);
                break;
            }
        }
    }
    
    for (uint32_t projId : projectilesToRemove) {
        projectiles.erase(projId);
        broadcast_projectile_despawn(projId);
    }
    
    for (uint32_t enemyId : enemiesToRemove) {
        enemies.erase(enemyId);
        broadcast_enemy_despawn(enemyId);
    }
}

void ServerGame::broadcast_projectile_positions() {
    for (const auto& kv : projectiles) {
        uint32_t id = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        
        ProjectileUpdateMessage msg;
        msg.type = MessageType::ProjectileUpdate;
        msg.projectileId = htonl(id);
        
        uint32_t xb, yb;
        std::memcpy(&xb, &x, sizeof(float));
        std::memcpy(&yb, &y, sizeof(float));
        
        msg.posXBits = htonl(xb);
        msg.posYBits = htonl(yb);
        
        connexion.broadcast(&msg, sizeof(msg));
    }
}

void ServerGame::broadcast_enemy_positions() {
    for (const auto& kv : enemies) {
        uint32_t id = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        broadcast_enemy_update(id, x, y);
    }
}

void ServerGame::update_projectiles_server_only(float dt) {
    std::vector<uint32_t> toRemove;
    
    for (auto& kv : projectiles) {
        uint32_t id = kv.first;
        float& x = std::get<0>(kv.second);
        float& y = std::get<1>(kv.second);
        float vx = std::get<2>(kv.second);
        float vy = std::get<3>(kv.second);
        x += vx * dt;
        y += vy * dt;

        if (x < -50.f || x > 850.f || y < -50.f || y > 650.f) {
            toRemove.push_back(id);
        }
    }
    for (uint32_t id : toRemove) {
        projectiles.erase(id);
        broadcast_projectile_despawn(id);
    }
}

void ServerGame::update_enemies(float dt) {
    const float SCREEN_WIDTH = 800.f;
    const float ENEMY_MARGIN = 20.f;
    
    for (auto& kv : enemies) {
        uint32_t id = kv.first;
        float& x = std::get<0>(kv.second);
        float& y = std::get<1>(kv.second);
        float& vx = std::get<2>(kv.second);
        float vy = std::get<3>(kv.second);
        x += vx * dt;
        y += vy * dt;
        if (x < ENEMY_MARGIN || x > SCREEN_WIDTH - ENEMY_MARGIN) {
            vx *= -1.f;
        }
    }
}

void ServerGame::broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId, 
                                    float x, float y, float vx, float vy) {
    ProjectileSpawnMessage msg;
    msg.type = MessageType::ProjectileSpawn;
    msg.projectileId = htonl(projId);
    msg.ownerId = htonl(ownerId);
    
    uint32_t xb, yb, vxb, vyb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&vxb, &vx, sizeof(float));
    std::memcpy(&vyb, &vy, sizeof(float));
    
    msg.posXBits = htonl(xb);
    msg.posYBits = htonl(yb);
    msg.velXBits = htonl(vxb);
    msg.velYBits = htonl(vyb);
    
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_projectile_despawn(uint32_t projId) {
    ProjectileDespawnMessage msg;
    msg.type = MessageType::ProjectileDespawn;
    msg.projectileId = htonl(projId);
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::initialize_enemies() {
    LOG_DEBUG("Initializing enemies...");    
    for (int i = 0; i < 5; ++i) {
        uint32_t enemyId = nextEnemyId++;
        float x = 100.f + i * 100.f;
        float y = 200.f;
        float vx = 50.f;
        float vy = 0.f;
        enemies[enemyId] = std::make_tuple(x, y, vx, vy);
        broadcast_enemy_spawn(enemyId, x, y, vx, vy);
        LOG_DEBUG("[Server] Enemy spawned: id=" << enemyId << " pos=(" << x << ", " << y << ")");
    }
}

void ServerGame::broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float vx, float vy) {
    EnemySpawnMessage msg;
    msg.type = MessageType::EnemySpawn;
    msg.enemyId = htonl(enemyId);
    
    uint32_t xb, yb, vxb, vyb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&vxb, &vx, sizeof(float));
    std::memcpy(&vyb, &vy, sizeof(float));
    
    msg.posXBits = htonl(xb);
    msg.posYBits = htonl(yb);
    msg.velXBits = htonl(vxb);
    msg.velYBits = htonl(vyb);
    
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_enemy_despawn(uint32_t enemyId) {
    EnemyDespawnMessage msg;
    msg.type = MessageType::EnemyDespawn;
    msg.enemyId = htonl(enemyId);
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_enemy_update(uint32_t enemyId, float x, float y) {
    EnemyUpdateMessage msg;
    msg.type = MessageType::EnemyUpdate;
    msg.enemyId = htonl(enemyId);
    
    uint32_t xb, yb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    
    msg.posXBits = htonl(xb);
    msg.posYBits = htonl(yb);
    
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_player_death(uint32_t clientId) {
    PlayerDeathMessage msg;
    msg.type = MessageType::PlayerDeath;
    msg.clientId = htonl(clientId);
    connexion.broadcast(&msg, sizeof(msg));
    LOG_INFO("[Server] Player " << clientId << " died!");
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
        LOG_DEBUG("===== Loaded Players =====");
        auto &positions = registry_server.get_components<component::position>();
        auto &clientIds = registry_server.get_components<component::client_id>();
        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].has_value() && clientIds[i].has_value()) {
                auto &pos = positions[i].value();
                auto &cid = clientIds[i].value();
                ecs::entity_t entity = registry_server.entity_from_index(i);
                LOG("Player entity " << entity
                          << " client_id=" << cid.id
                          << " pos=(" << pos.x << ", " << pos.y << ")");
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
        LOG_DEBUG("===== Loaded Level Entities =====");
        auto &positions = registry_server.get_components<component::position>();
        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (positions[i].has_value()) {
                auto &pos = positions[i].value();
                ecs::entity_t entity = registry_server.entity_from_index(i);
                LOG("Entity " << entity
                          << " pos=(" << pos.x << ", " << pos.y << ")");
            }
        }
    } catch (const std::exception &e) {
        LOG_ERROR("load_level failed: " << e.what());
    }
}

void ServerGame::initialize_player_positions() {
    LOG_DEBUG("Initializing player positions...");
    for (const auto& kv : connexion.getClients()) {
        playerPositions[kv.second] = {100.f + (kv.second - 1) * 50.f, 300.f};
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

bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                        const std::unordered_map<uint32_t, std::tuple<float, float, float, float>>& obstacles) {
    float halfW = playerWidth * 0.5f;
    float halfH = playerHeight * 0.5f;
    float left   = testX - halfW;
    float right  = testX + halfW;
    float top    = testY - halfH;
    float bottom = testY + halfH;
    for (const auto& kv : obstacles) {
        float obsX = std::get<0>(kv.second);
        float obsY = std::get<1>(kv.second);
        float obsW = std::get<2>(kv.second);
        float obsH = std::get<3>(kv.second);
        float obsLeft   = obsX - obsW * 0.5f;
        float obsRight  = obsX + obsW * 0.5f;
        float obsTop    = obsY - obsH * 0.5f;
        float obsBottom = obsY + obsH * 0.5f;
        if (check_aabb_overlap(left, right, top, bottom,
                              obsLeft, obsRight, obsTop, obsBottom)) {
            return true;
        }
    }
    return false;
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
                float inputX, inputY;
                uint32_t xbits = ntohl(msg->inputXBits);
                uint32_t ybits = ntohl(msg->inputYBits);
                std::memcpy(&inputX, &xbits, sizeof(float));
                std::memcpy(&inputY, &ybits, sizeof(float));
                auto& pos = playerPositions[id];
                float speed = 200.f / 60.f;
                const float playerWidth = 30.f;
                const float playerHeight = 30.f;
                float newX = pos.first + inputX * speed;
                float newY = pos.second + inputY * speed;
                if (!is_position_blocked(newX, pos.second, playerWidth, playerHeight, obstacles))
                    pos.first = newX;
                if (!is_position_blocked(pos.first, newY, playerWidth, playerHeight, obstacles))
                    pos.second = newY;
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
                    float projVelX = 400.f;
                    float projVelY = 0.f;
                    
                    projectiles[projId] = std::make_tuple(projX, projY, projVelX, projVelY, clientId);
                    broadcast_projectile_spawn(projId, clientId, projX, projY, projVelX, projVelY);
                    
                    LOG_DEBUG("[Server] Client " << clientId << " shot projectile " << projId);
                }
            }
            break;
        }
        default:
            break;
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
        uint32_t xbits, ybits;
        std::memcpy(&xbits, &it->second.first, sizeof(float));
        std::memcpy(&ybits, &it->second.second, sizeof(float));
        m.posXBits = htonl(xbits);
        m.posYBits = htonl(ybits);
        connexion.broadcast(&m, sizeof(m));
    }
}

void ServerGame::initialize_obstacles() {
    LOG_DEBUG("Initializing obstacles...");
    struct O { float x,y,w,h; }; O list[3] = {
        {200.f, 400.f, 60.f, 60.f},
        {400.f, 400.f, 60.f, 60.f},
        {600.f, 400.f, 60.f, 60.f}
    };
    uint32_t oid = 1;
    for (auto &o : list) {
        obstacles[oid] = std::make_tuple(o.x, o.y, o.w, o.h);
        broadcast_obstacle_spawn(oid, o.x, o.y, o.w, o.h);
        ++oid;
    }
}

void ServerGame::broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h) {
    ObstacleSpawnMessage m;
    m.type = MessageType::ObstacleSpawn;
    m.obstacleId = htonl(obstacleId);
    uint32_t xb, yb, wb, hb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&wb, &w, sizeof(float));
    std::memcpy(&hb, &h, sizeof(float));
    m.posXBits = htonl(xb);
    m.posYBits = htonl(yb);
    m.widthBits = htonl(wb);
    m.heightBits = htonl(hb);
    connexion.broadcast(&m, sizeof(m));
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
