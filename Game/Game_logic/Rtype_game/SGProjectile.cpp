/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGProjectile
*/

#include "../Rtype_game.hpp"
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


template <typename Component>
inline Component* get_component_ptr(ecs::registry &registry, ecs::entity_t entity) {
    auto &arr = registry.get_components<Component>();
    auto idx = static_cast<std::size_t>(entity);
    if (idx >= arr.size() || !arr[idx].has_value()) {
        return nullptr;
    }
    return &arr[idx].value();
}

void ServerGame::check_projectile_enemy_collisions() {
    std::vector<uint32_t> projectilesToRemove;
    std::vector<uint32_t> enemiesToRemove;
    const float PROJ_WIDTH = 10.f;
    const float PROJ_HEIGHT = 5.f;
    const int DAMAGE_PER_HIT = 1;

    auto &collision_boxes = registry_server.get_components<component::collision_box>();

    for (const auto& projKv : projectiles) {
        uint32_t projId = projKv.first;
        float projX = std::get<0>(projKv.second);
        float projY = std::get<1>(projKv.second);

        float projLeft = projX - PROJ_WIDTH * 0.5f;
        float projRight = projX + PROJ_WIDTH * 0.5f;
        float projTop = projY - PROJ_HEIGHT * 0.5f;
        float projBottom = projY + PROJ_HEIGHT * 0.5f;

        for (auto enemyEntity : _enemies) {
            uint32_t eid = static_cast<uint32_t>(enemyEntity);
            auto pos = get_component_ptr<component::position>(registry_server, enemyEntity);
            auto health = get_component_ptr<component::health>(registry_server, enemyEntity);
            if (!pos || !health)
                continue;

            float enemyWidth = 30.f;
            float enemyHeight = 30.f;
            if (eid < collision_boxes.size() && collision_boxes[eid]) {
                enemyWidth = collision_boxes[eid]->width;
                enemyHeight = collision_boxes[eid]->height;
            }
            
            float enemyX = pos->x;
            float enemyY = pos->y;
            float enemyLeft = enemyX - enemyWidth * 0.5f;
            float enemyRight = enemyX + enemyWidth * 0.5f;
            float enemyTop = enemyY - enemyHeight * 0.5f;
            float enemyBottom = enemyY + enemyHeight * 0.5f;

            if (check_aabb_overlap(projLeft, projRight, projTop, projBottom,
                                  enemyLeft, enemyRight, enemyTop, enemyBottom)) {
                
                projectilesToRemove.push_back(projId);
                
                health->current -= DAMAGE_PER_HIT;
                
                uint32_t enemyId = static_cast<uint32_t>(enemyEntity);
                uint32_t killerId = std::get<6>(projKv.second);
                
                if (health->current <= 0) {
                    enemiesToRemove.push_back(enemyId);
                    totalScore += 10;
                    if (playerIndividualScores.find(killerId) == playerIndividualScores.end()) {
                        playerIndividualScores[killerId] = 0;
                    }
                    playerIndividualScores[killerId] += 10;
                }
                break;
            }
        }
    }

    for (uint32_t projId : projectilesToRemove) {
        projectiles.erase(projId);
        broadcast_projectile_despawn(projId);
    }

    for (uint32_t enemyId : enemiesToRemove) {
        registry_server.kill_entity(static_cast<ecs::entity_t>(enemyId));
        broadcast_enemy_despawn(enemyId);
        _enemies.erase(std::remove(_enemies.begin(), _enemies.end(), 
                                   static_cast<ecs::entity_t>(enemyId)), _enemies.end());
    }
}
void ServerGame::broadcast_projectile_positions() {
    for (const auto& kv : projectiles) {
        uint32_t id = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        float z = std::get<2>(kv.second);

        ProjectileUpdateMessage msg;
        msg.type = MessageType::ProjectileUpdate;
        msg.projectileId = htonl(id);

        uint32_t xb, yb, zb;
        std::memcpy(&xb, &x, sizeof(float));
        std::memcpy(&yb, &y, sizeof(float));
        std::memcpy(&zb, &z, sizeof(float));

        msg.pos.xBits = htonl(xb);
        msg.pos.yBits = htonl(yb);
        msg.pos.zBits = htonl(zb);

        connexion.broadcast(&msg, sizeof(msg));
    }
}

void ServerGame::update_projectiles_server_only(float dt) {
    std::vector<uint32_t> toRemove;

    for (auto& kv : projectiles) {
        uint32_t id = kv.first;
        float& x = std::get<0>(kv.second);
        float& y = std::get<1>(kv.second);
        float& z = std::get<2>(kv.second);
        float vx = std::get<3>(kv.second);
        float vy = std::get<4>(kv.second);
        float vz = std::get<5>(kv.second);
        
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
        
        if (x < -50.f || x > 1970.f || y < -50.f || y > 1130.f) {
            toRemove.push_back(id);
        }
    }
    for (uint32_t id : toRemove) {
        projectiles.erase(id);
        broadcast_projectile_despawn(id);
    }
}

void ServerGame::broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId,
                                    float x, float y, float z, float vx, float vy, float vz) {
    ProjectileSpawnMessage msg;
    msg.type = MessageType::ProjectileSpawn;
    msg.projectileId = htonl(projId);
    msg.ownerId = htonl(ownerId);

    uint32_t xb, yb, zb, vxb, vyb, vzb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&zb, &z, sizeof(float));
    std::memcpy(&vxb, &vx, sizeof(float));
    std::memcpy(&vyb, &vy, sizeof(float));
    std::memcpy(&vzb, &vz, sizeof(float));

    msg.pos.xBits = htonl(xb);
    msg.pos.yBits = htonl(yb);
    msg.pos.zBits = htonl(zb);
    msg.vel.vxBits = htonl(vxb);
    msg.vel.vyBits = htonl(vyb);
    msg.vel.vzBits = htonl(vzb);

    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_projectile_despawn(uint32_t projId) {
    ProjectileDespawnMessage msg;
    msg.type = MessageType::ProjectileDespawn;
    msg.projectileId = htonl(projId);
    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::check_projectile_collisions() {
    std::vector<uint32_t> projectilesToRemove;

    const float PROJ_WIDTH = 10.f;
    const float PROJ_HEIGHT = 5.f;

    for (const auto& projKv : projectiles) {
        uint32_t projId = projKv.first;
        float projX = std::get<0>(projKv.second);
        float projY = std::get<1>(projKv.second);

        float projLeft = projX - PROJ_WIDTH * 0.5f;
        float projRight = projX + PROJ_WIDTH * 0.5f;
        float projTop = projY - PROJ_HEIGHT * 0.5f;
        float projBottom = projY + PROJ_HEIGHT * 0.5f;

        for (auto obstacleEntity : _obstacles) {
            auto pos = get_component_ptr<component::position>(registry_server, obstacleEntity);
            auto box = get_component_ptr<component::collision_box>(registry_server, obstacleEntity);

            if (!pos || !box)
                continue;

            float obsX = pos->x;
            float obsY = pos->y;
            float obsW = box->width;
            float obsH = box->height;

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
}

void ServerGame::shoot_enemy_projectile(uint32_t enemyId, float x, float y, float vx, float vy) {
    uint32_t projId = nextEnemyProjectileId++;
    
    float spawnX = x - 20.f;
    float spawnY = y;
    float spawnZ = 0.f;
    float velZ = 0.f;
    
    enemyProjectiles[projId] = std::make_tuple(spawnX, spawnY, spawnZ, vx, vy, velZ, enemyId);
    broadcast_enemy_projectile_spawn(projId, enemyId, spawnX, spawnY, spawnZ, vx, vy, velZ);
}

void ServerGame::update_enemy_projectiles_server_only(float dt) {
    std::vector<uint32_t> toRemove;

    for (auto& kv : enemyProjectiles) {
        uint32_t id = kv.first;
        float& x = std::get<0>(kv.second);
        float& y = std::get<1>(kv.second);
        float& z = std::get<2>(kv.second);
        float vx = std::get<3>(kv.second);
        float vy = std::get<4>(kv.second);
        float vz = std::get<5>(kv.second);
        
        x += vx * dt;
        y += vy * dt;
        z += vz * dt;
        
        if (x < -50.f || x > 1970.f || y < -50.f || y > 1130.f) {
            toRemove.push_back(id);
        }
    }
    for (uint32_t id : toRemove) {
        enemyProjectiles.erase(id);
        broadcast_enemy_projectile_despawn(id);
    }
}

void ServerGame::check_enemy_projectile_player_collisions() {
    std::vector<uint32_t> projectilesToRemove;
    std::vector<uint32_t> playersHit;
    
    const float PLAYER_WIDTH = 30.f;
    const float PLAYER_HEIGHT = 30.f;
    const float PROJ_WIDTH = 10.f;
    const float PROJ_HEIGHT = 5.f;
    const int DAMAGE = 15;
    
    for (const auto& projKv : enemyProjectiles) {
        uint32_t projId = projKv.first;
        float projX = std::get<0>(projKv.second);
        float projY = std::get<1>(projKv.second);
        
        float projLeft = projX - PROJ_WIDTH * 0.5f;
        float projRight = projX + PROJ_WIDTH * 0.5f;
        float projTop = projY - PROJ_HEIGHT * 0.5f;
        float projBottom = projY + PROJ_HEIGHT * 0.5f;
        
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
            
            if (check_aabb_overlap(projLeft, projRight, projTop, projBottom,
                                  playerLeft, playerRight, playerTop, playerBottom)) {
                projectilesToRemove.push_back(projId);
                playersHit.push_back(playerId);
                LOG_DEBUG("[Server] Enemy projectile " << projId << " hit player " << playerId);
                break;
            }
        }
    }
    
    for (uint32_t projId : projectilesToRemove) {
        enemyProjectiles.erase(projId);
        broadcast_enemy_projectile_despawn(projId);
    }
    
    for (uint32_t playerId : playersHit) {
        auto& healths = registry_server.get_components<component::health>();
        auto& clientIds = registry_server.get_components<component::client_id>();
        
        for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
            if (clientIds[i] && clientIds[i]->id == playerId && healths[i]) {
                healths[i]->current -= DAMAGE;
                LOG_INFO("[Server] Player " << playerId << " hit by enemy projectile! Health: " 
                        << healths[i]->current << "/" << healths[i]->max);
                
                if (healths[i]->current <= 0) {
                    deadPlayers.insert(playerId);
                    broadcast_player_death(playerId);
                }
                break;
            }
        }
    }
}

void ServerGame::broadcast_enemy_projectile_spawn(uint32_t projId, uint32_t ownerId,
                                                   float x, float y, float z, float vx, float vy, float vz) {
    EnemyProjectileSpawnMessage msg;
    msg.type = MessageType::EnemyProjectileSpawn;
    msg.projectileId = htonl(projId);
    msg.ownerId = htonl(ownerId);

    uint32_t xb, yb, zb, vxb, vyb, vzb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&zb, &z, sizeof(float));
    std::memcpy(&vxb, &vx, sizeof(float));
    std::memcpy(&vyb, &vy, sizeof(float));
    std::memcpy(&vzb, &vz, sizeof(float));

    msg.pos.xBits = htonl(xb);
    msg.pos.yBits = htonl(yb);
    msg.pos.zBits = htonl(zb);
    msg.vel.vxBits = htonl(vxb);
    msg.vel.vyBits = htonl(vyb);
    msg.vel.vzBits = htonl(vzb);

    connexion.broadcast(&msg, sizeof(msg));
}

void ServerGame::broadcast_enemy_projectile_positions() {
    for (const auto& kv : enemyProjectiles) {
        uint32_t id = kv.first;
        float x = std::get<0>(kv.second);
        float y = std::get<1>(kv.second);
        float z = std::get<2>(kv.second);

        EnemyProjectileUpdateMessage msg;
        msg.type = MessageType::EnemyProjectileUpdate;
        msg.projectileId = htonl(id);

        uint32_t xb, yb, zb;
        std::memcpy(&xb, &x, sizeof(float));
        std::memcpy(&yb, &y, sizeof(float));
        std::memcpy(&zb, &z, sizeof(float));

        msg.pos.xBits = htonl(xb);
        msg.pos.yBits = htonl(yb);
        msg.pos.zBits = htonl(zb);

        connexion.broadcast(&msg, sizeof(msg));
    }
}

void ServerGame::broadcast_enemy_projectile_despawn(uint32_t projId) {
    EnemyProjectileDespawnMessage msg;
    msg.type = MessageType::EnemyProjectileDespawn;
    msg.projectileId = htonl(projId);
    connexion.broadcast(&msg, sizeof(msg));
}