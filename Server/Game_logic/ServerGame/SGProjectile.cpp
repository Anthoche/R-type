/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGProjectile
*/
#include "../Include/ServerGame.hpp"
#include "../../../Engine/Utils/Include/serializer.hpp"
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

    const float ENEMY_WIDTH = 30.f;
    const float ENEMY_HEIGHT = 30.f;
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

        for (auto enemyEntity : _enemies) {
            auto pos = get_component_ptr<component::position>(registry_server, enemyEntity);
            if (!pos)
                continue;

            float enemyX = pos->x;
            float enemyY = pos->y;
            float enemyLeft = enemyX - ENEMY_WIDTH * 0.5f;
            float enemyRight = enemyX + ENEMY_WIDTH * 0.5f;
            float enemyTop = enemyY - ENEMY_HEIGHT * 0.5f;
            float enemyBottom = enemyY + ENEMY_HEIGHT * 0.5f;

            if (check_aabb_overlap(projLeft, projRight, projTop, projBottom,
                                  enemyLeft, enemyRight, enemyTop, enemyBottom)) {
                projectilesToRemove.push_back(projId);
                enemiesToRemove.push_back(static_cast<uint32_t>(enemyEntity));
                LOG_DEBUG("[Server] Projectile " << projId << " hit enemy " << static_cast<uint32_t>(enemyEntity));
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
        _enemies.erase(std::remove(_enemies.begin(), _enemies.end(), static_cast<ecs::entity_t>(enemyId)), _enemies.end());
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