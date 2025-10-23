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
        auto pattern_comp = get_component_ptr<component::pattern_element>(registry_server, entity);
        if (pattern_comp && !pattern_comp->pattern_name.empty()) {
            pattern = pattern_comp->pattern_name;
        }        

        if (pattern == "straight") update_enemy_straight(id, dt);
        else if (pattern == "zigzag") update_enemy_zigzag(id, dt);
        else if (pattern == "circle") update_enemy_circle(id, dt);
        else if (pattern == "turret") update_enemy_turret(id, dt, 3.0f);
        else if (pattern == "spiral") update_enemy_spiral(id, dt);
        else if (pattern == "figure") update_enemy_figure8(id, dt);
        else if (pattern == "boss_phase1") update_enemy_boss_phase1(id, dt);
        else update_enemy_default(id, dt);

        pos = get_component_ptr<component::position>(registry_server, entity);
        if (pos) broadcast_enemy_update(id, pos->x, pos->y, pos->z);
    }
}

void ServerGame::update_enemy_default(uint32_t id, float dt) {
    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    float vx = -50.f, vy = 0.f, vz = 0.f;
    if (vel) { vx = vel->vx; vy = vel->vy; vz = vel->vz; }
    pos->x += vx * dt;
    pos->y += vy * dt;
    pos->z += vz * dt;
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
    static std::unordered_map<uint32_t, float> initialX;
    static std::unordered_map<uint32_t, float> time;

    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    if (angles.find(id) == angles.end()) {
        angles[id] = 0.f;
        initialX[id] = pos->x;
        time[id] = 0.f;
    }

    time[id] += dt;
    float baseVelocityX = -50.f;
    if (vel) baseVelocityX = vel->vx;
    const float circleRadius = 25.f;
    const float angularSpeed = 4.0f;
    const float waveAmplitude = 20.f;
    const float waveFrequency = 0.5f;
    angles[id] += angularSpeed * dt;
    initialX[id] += baseVelocityX * dt;
    float baseY = 540.f + std::sin(time[id] * waveFrequency) * waveAmplitude;
    pos->x = initialX[id] + std::cos(angles[id]) * circleRadius;
    pos->y = baseY + std::sin(angles[id]) * circleRadius;
}

void ServerGame::update_enemy_spiral(uint32_t id, float dt) {
    static std::unordered_map<uint32_t, float> angles;
    static std::unordered_map<uint32_t, float> initialX;
    static std::unordered_map<uint32_t, float> initialY;
    static std::unordered_map<uint32_t, float> currentRadius;

    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    if (angles.find(id) == angles.end()) {
        angles[id] = 0.f;
        initialX[id] = pos->x;
        initialY[id] = pos->y;
        currentRadius[id] = 10.f;
    }

    float baseVelocityX = -50.f;
    if (vel) baseVelocityX = vel->vx;
    const float angularSpeed = 3.0f;
    const float radiusGrowth = 15.f;
    const float maxRadius = 80.f;
    angles[id] += angularSpeed * dt;
    initialX[id] += baseVelocityX * dt;
    currentRadius[id] = std::min(currentRadius[id] + radiusGrowth * dt, maxRadius);
    pos->x = initialX[id] + std::cos(angles[id]) * currentRadius[id];
    pos->y = initialY[id] + std::sin(angles[id]) * currentRadius[id];
    update_enemy_turret(id, dt, 1.0f);
}

void ServerGame::update_enemy_figure8(uint32_t id, float dt) {
    static std::unordered_map<uint32_t, float> time;
    static std::unordered_map<uint32_t, float> initialX;
    static std::unordered_map<uint32_t, float> initialY;

    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    if (time.find(id) == time.end()) {
        time[id] = 0.f;
        initialX[id] = pos->x;
        initialY[id] = pos->y;
    }

    time[id] += dt;
    float baseVelocityX = -50.f;
    if (vel) baseVelocityX = vel->vx;
    const float scale = 40.f;
    const float speed = 2.0f;
    initialX[id] += baseVelocityX * dt;
    float t = time[id] * speed;
    float denominator = 1.f + std::sin(t) * std::sin(t);
    pos->x = initialX[id] + (scale * std::cos(t)) / denominator;
    pos->y = initialY[id] + (scale * std::sin(t) * std::cos(t)) / denominator;
    update_enemy_turret(id, dt, 2.0f);
}

void ServerGame::update_enemy_turret(uint32_t id, float dt, float rapidfire) {
    static std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> lastShootTime;
    static std::unordered_map<uint32_t, float> shootCooldowns;
    static std::unordered_map<uint32_t, bool> initialized;
    
    ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    float vx = -30.f, vy = 0.f, vz = 0.f;
    if (vel) { vx = vel->vx; vy = vel->vy; vz = vel->vz; }
    pos->x += vx * dt;
    pos->y += vy * dt;
    pos->z += vz * dt;
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    
    if (!initialized[id]) {
        float cooldown = 10.0f / rapidfire;
        float randomOffset = (static_cast<float>(rand() % 1000) / 1000.0f) * cooldown;        
        lastShootTime[id] = currentTime - std::chrono::milliseconds(
            static_cast<int>(randomOffset * 1000)
        );
        shootCooldowns[id] = cooldown;
        initialized[id] = true;
    }
    
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(
        currentTime - lastShootTime[id]
    ).count();
    
    if (elapsed >= shootCooldowns[id]) {
        float enemyWidth = 30.f;
        auto cbox = get_component_ptr<component::collision_box>(registry_server, entity);
        if (cbox) {
            enemyWidth = cbox->width;
        }        
        float shootX = pos->x - (enemyWidth / 2.0f);
        shoot_enemy_projectile(id, shootX, pos->y, -200.f, 0.f);
        lastShootTime[id] = currentTime;
    }
}

void ServerGame::update_enemy_boss_phase1(uint32_t id, float dt) {
ecs::entity_t entity = static_cast<ecs::entity_t>(id);
    auto pos = get_component_ptr<component::position>(registry_server, entity);
    auto vel = get_component_ptr<component::velocity>(registry_server, entity);
    if (!pos) return;

    float baseVx = -80.f;
    if (vel) baseVx = vel->vx;

    const float amplitude = 230.f;
    const float wavelength = 50.f;
    pos->x += baseVx * dt;
    pos->y += std::sin(pos->x / wavelength) * amplitude * dt;
    update_enemy_turret(id, dt, 6.0f);
}

void ServerGame::check_player_enemy_collisions() {
    const float PLAYER_WIDTH = 30.f;
    const float PLAYER_HEIGHT = 30.f;
    const int DAMAGE_PER_HIT = 10;
    const int COOLDOWN_MS = 1000;
    
    std::vector<uint32_t> playersHit;
    auto now = std::chrono::high_resolution_clock::now();
    
    auto &collision_boxes = registry_server.get_components<component::collision_box>();
    
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
            float enemyWidth = 30.f;
            float enemyHeight = 30.f;
            if (eid < collision_boxes.size() && collision_boxes[eid]) {
                enemyWidth = collision_boxes[eid]->width;
                enemyHeight = collision_boxes[eid]->height;
            }
            
            float enemyX = epos->x;
            float enemyY = epos->y;
            
            float enemyLeft = enemyX - enemyWidth * 0.5f;
            float enemyRight = enemyX + enemyWidth * 0.5f;
            float enemyTop = enemyY - enemyHeight * 0.5f;
            float enemyBottom = enemyY + enemyHeight * 0.5f;
            
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
            const auto& pos = *positions[enemyId];
            broadcast_enemy_update(enemyId, pos.x, pos.y, pos.z);
        }
    }
}

void ServerGame::broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float z, float vx, float vy, float vz, float width, float height) {
    EnemySpawnMessage msg{};
    msg.type = MessageType::EnemySpawn;
    msg.enemyId = htonl(enemyId);

    uint32_t xb, yb, zb, vxb, vyb, vzb, w, h;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&zb, &z, sizeof(float));
    std::memcpy(&vxb, &vx, sizeof(float));
    std::memcpy(&vyb, &vy, sizeof(float));
    std::memcpy(&vzb, &vz, sizeof(float));
    std::memcpy(&w, &width, sizeof(float));
    std::memcpy(&h, &height, sizeof(float));

    msg.pos.xBits = htonl(xb);
    msg.pos.yBits = htonl(yb);
    msg.pos.zBits = htonl(zb);
    msg.vel.vxBits = htonl(vxb);
    msg.vel.vyBits = htonl(vyb);
    msg.vel.vzBits = htonl(vzb);
    msg.width = htonl(w);
    msg.height = htonl(h);

    connexion.broadcast(&msg, sizeof(msg));
    LOG_DEBUG("[Server] Broadcast enemy spawn: ID=" << enemyId << " pos=(" << x << "," << y << "," << z << ")");
}

void ServerGame::broadcast_enemy_despawn(uint32_t enemyId) {
    EnemyDespawnMessage msg{};
    msg.type = MessageType::EnemyDespawn;
    msg.enemyId = htonl(enemyId);

    connexion.broadcast(&msg, sizeof(msg));
    LOG_DEBUG("[Server] Broadcast enemy despawn: ID=" << enemyId);
}

void ServerGame::broadcast_boss_death(uint32_t bossId) {
    BossDeathMessage msg{};
    msg.type = MessageType::BossDeath;
    msg.bossId = htonl(bossId);
    
    connexion.broadcast(&msg, sizeof(msg));
    levelTransitionPending = true;
    levelTransitionTime = std::chrono::steady_clock::now();
}

void ServerGame::broadcast_enemy_update(uint32_t enemyId, float x, float y, float z) {
    EnemyUpdateMessage msg{};
    msg.type = MessageType::EnemyUpdate;
    msg.enemyId = htonl(enemyId);

    uint32_t xb, yb, zb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&zb, &z, sizeof(float));

    msg.pos.xBits = htonl(xb);
    msg.pos.yBits = htonl(yb);
    msg.pos.zBits = htonl(zb);

    uint32_t vxb = 0;
    uint32_t vyb = 0;
    auto& velocities = registry_server.get_components<component::velocity>();
    if (enemyId < velocities.size() && velocities[enemyId]) {
        float vx = velocities[enemyId]->vx;
        float vy = velocities[enemyId]->vy;
        std::memcpy(&vxb, &vx, sizeof(float));
        std::memcpy(&vyb, &vy, sizeof(float));
    }

    msg.velXBits = htonl(vxb);
    msg.velYBits = htonl(vyb);

    connexion.broadcast(&msg, sizeof(msg));
}