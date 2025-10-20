/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGEnnemy
*/


#include "../Rtype_game.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <cmath>
#include <chrono>

#define RESET_COLOR   "\033[0m"
#define RED_COLOR     "\033[31m"
#define GREEN_COLOR   "\033[32m"
#define YELLOW_COLOR  "\033[33m"
#define BLUE_COLOR    "\033[34m"
#define LOG_ERROR(msg)   std::cerr << RED_COLOR << "[ERROR] " << msg << RESET_COLOR << std::endl
#define LOG_INFO(msg)    std::cout << GREEN_COLOR << "[INFO] " << msg << RESET_COLOR << std::endl
#define LOG_DEBUG(msg)   std::cout << YELLOW_COLOR << "[DEBUG] " << msg << RESET_COLOR << std::endl
#define LOG(msg)         std::cout << BLUE_COLOR << msg << RESET_COLOR << std::endl

// Helper pour récupérer un composant via registry actuelle
template <typename Component>
inline Component* get_component_ptr(ecs::registry &registry, ecs::entity_t entity) {
    auto &arr = registry.get_components<Component>();
    auto idx = static_cast<std::size_t>(entity);
    if (idx >= arr.size() || !arr[idx].has_value()) {
        return nullptr;
    }
    return &arr[idx].value();
}

void ServerGame::update_enemies(float dt) {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<ecs::entity_t> enemiesCopy = _enemies;

    for (auto entity : enemiesCopy) {
        auto pos = get_component_ptr<component::position>(registry_server, entity);
        if (!pos) continue;

        uint32_t id = static_cast<uint32_t>(entity);
        std::string pattern = "default";
        auto it = enemyPatterns.find(id);
        if (it != enemyPatterns.end()) pattern = it->second;

        if (pattern == "straight") update_enemy_straight(id, dt);
        else if (pattern == "zigzag") update_enemy_zigzag(id, dt);
        else if (pattern == "circle") update_enemy_circle(id, dt);
        else if (pattern == "boss_phase1") update_enemy_boss_phase1(id, dt);
        else update_enemy_default(id, dt);
        update_enemy_turret(id, dt);

        pos = get_component_ptr<component::position>(registry_server, entity);
        if (pos) broadcast_enemy_update(id, pos->x, pos->y);
    }
}

void ServerGame::update_enemy_default(uint32_t id, float dt) {
    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    float vx = -50.f, vy = 0.f;
    if (vel) { vx = vel->vx; vy = vel->vy; }
    pos->x += vx * dt;
    pos->y += vy * dt;
}

void ServerGame::update_enemy_straight(uint32_t id, float dt) {
    update_enemy_default(id, dt);
}

void ServerGame::update_enemy_zigzag(uint32_t id, float dt) {
    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    float baseVx = -80.f;
    if (vel) baseVx = vel->vx;

    const float amplitude = 30.f;
    const float wavelength = 50.f;
    pos->x += baseVx * dt;
    pos->y += std::sin(pos->x / wavelength) * amplitude * dt;
}

void ServerGame::update_enemy_circle(uint32_t id, float dt) {
    static std::unordered_map<uint32_t, float> angles;
    static std::unordered_map<uint32_t, std::pair<float, float>> centers;

    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    if (centers.find(id) == centers.end()) {
        centers[id] = std::make_pair(pos->x, pos->y);
        angles[id] = 0.f;
    }

    float radius = 100.f;
    float speed = 1.f;
    if (vel) {
        float mag = std::sqrt(vel->vx*vel->vx + vel->vy*vel->vy);
        if (mag > 10.f) radius = mag;
        speed = radius / 50.f;
    }

    angles[id] += speed * dt;
    const auto &c = centers[id];
    pos->x = c.first + std::cos(angles[id]) * radius;
    pos->y = c.second + std::sin(angles[id]) * radius;
}

void ServerGame::update_enemy_turret(uint32_t id, float dt) {
    static std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> lastShootTime;
    static const float SHOOT_COOLDOWN = 2.0f;
    
    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    float vx = -30.f, vy = 0.f;
    if (vel) { vx = vel->vx; vy = vel->vy; }
    pos->x += vx * dt;
    pos->y += vy * dt;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    if (lastShootTime.find(id) == lastShootTime.end()) {
        lastShootTime[id] = currentTime;
    }
    
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(
        currentTime - lastShootTime[id]
    ).count();
    
    if (elapsed >= SHOOT_COOLDOWN) {
        shoot_enemy_projectile(id, pos->x, pos->y, -200.f, 0.f);
        lastShootTime[id] = currentTime;
    }
}

void ServerGame::update_enemy_boss_phase1(uint32_t id, float dt) {
    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    float vx = -20.f;
    if (vel) vx = vel->vx;

    const float amplitude = 40.f;
    const float freq = 0.5f;
    pos->x += vx * dt;
    pos->y += std::sin(pos->x * freq) * amplitude * dt;
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
        
        for (const auto& enemy : _enemies) {
            uint32_t eid = static_cast<uint32_t>(enemy);
            auto epos = get_component_ptr<component::position>(registry_server, enemy);
            if (!epos)
                continue;
            float enemyX = epos->x;
            float enemyY = epos->y;
            
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

void ServerGame::broadcast_enemy_positions() {
    auto &positions = registry_server.get_components<component::position>();
    
    for (auto enemyEntity : _enemies) {
        uint32_t enemyId = static_cast<uint32_t>(enemyEntity);
        
        if (enemyId < positions.size() && positions[enemyId]) {
            float x = positions[enemyId]->x;
            float y = positions[enemyId]->y;
            
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
    }
}

void ServerGame::broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float vx, float vy) {
    EnemySpawnMessage msg{};
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
    LOG_DEBUG("[Server] Broadcast enemy spawn: ID=" << enemyId << " pos=(" << x << "," << y << ")");
}

void ServerGame::broadcast_enemy_despawn(uint32_t enemyId) {
    EnemyDespawnMessage msg{};
    msg.type = MessageType::EnemyDespawn;
    msg.enemyId = htonl(enemyId);

    connexion.broadcast(&msg, sizeof(msg));
    LOG_DEBUG("[Server] Broadcast enemy despawn: ID=" << enemyId);
}

void ServerGame::broadcast_enemy_update(uint32_t enemyId, float x, float y) {
    EnemyUpdateMessage msg{};
    msg.type = MessageType::EnemyUpdate;
    msg.enemyId = htonl(enemyId);

    uint32_t xb, yb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));

    msg.posXBits = htonl(xb);
    msg.posYBits = htonl(yb);

    connexion.broadcast(&msg, sizeof(msg));
}
