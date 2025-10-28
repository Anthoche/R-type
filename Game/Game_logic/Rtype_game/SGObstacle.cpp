/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGObstacle
*/


#include "../Rtype_game.hpp"
#include <cmath>
#include <iostream>
#include <cstring>

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
inline Component* get_component_ptr_obstacle(ecs::registry &registry, ecs::entity_t entity) {
    auto &arr = registry.get_components<Component>();
    auto idx = static_cast<std::size_t>(entity);
    if (idx >= arr.size() || !arr[idx].has_value()) {
        return nullptr;
    }
    return &arr[idx].value();
}

void ServerGame::initialize_obstacles() {}

bool ServerGame::is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                                     const std::vector<ecs::entity_t> &obstacles)
{
    for (auto entity : obstacles) {
        auto pos = get_component_ptr_obstacle<component::position>(registry_server, entity);
        auto box = get_component_ptr_obstacle<component::collision_box>(registry_server, entity);

        if (!pos || !box)
            continue;

        float ox = pos->x;
        float oy = pos->y;
        float ow = box->width;
        float oh = box->height;

        float pLeft   = testX - playerWidth / 2.f;
        float pRight  = testX + playerWidth / 2.f;
        float pTop    = testY - playerHeight / 2.f;
        float pBottom = testY + playerHeight / 2.f;

        float oLeft   = ox - ow / 2.f;
        float oRight  = ox + ow / 2.f;
        float oTop    = oy - oh / 2.f;
        float oBottom = oy + oh / 2.f;

        if (check_aabb_overlap(pLeft, pRight, pTop, pBottom,
                               oLeft, oRight, oTop, oBottom)) {
            return true;
        }
    }
    return false;
}

void ServerGame::broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float z, 
    float w, float h, float d, float vx, float vy, float vz) {
    ObstacleSpawnMessage msg{};
    msg.type = MessageType::ObstacleSpawn;
    msg.obstacleId = htonl(obstacleId);

    uint32_t xb, yb, zb, wb, hb, db, vxb, vyb, vzb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&zb, &z, sizeof(float));
    std::memcpy(&wb, &w, sizeof(float));
    std::memcpy(&hb, &h, sizeof(float));
    std::memcpy(&db, &d, sizeof(float));
    std::memcpy(&vxb, &vx, sizeof(float));
    std::memcpy(&vyb, &vy, sizeof(float));
    std::memcpy(&vzb, &vz, sizeof(float));

    msg.pos.xBits = htonl(xb);
    msg.pos.yBits = htonl(yb);
    msg.pos.zBits = htonl(zb);
    msg.size.widthBits = htonl(wb);
    msg.size.heightBits = htonl(hb);
    msg.size.depthBits = htonl(db);
    msg.vel.vxBits = htonl(vxb);
    msg.vel.vyBits = htonl(vyb);
    msg.vel.vzBits = htonl(vzb);

    connexion.broadcast(&msg, sizeof(msg));
    LOG_DEBUG("[Server] Broadcast obstacle spawn: ID=" << obstacleId
              << " pos=(" << x << "," << y << "," << z << ") size=(" << w << "," << h << "," << d << ")");
}

void ServerGame::update_obstacles(float dt) {
    auto &positions = registry_server.get_components<component::position>();
    auto &velocities = registry_server.get_components<component::velocity>();
    
    for (auto obstacle : _obstacles) {
        uint32_t idx = static_cast<uint32_t>(obstacle);
        
        if (idx >= positions.size() || !positions[idx]) continue;
        if (idx >= velocities.size() || !velocities[idx]) continue;
        
        positions[idx]->x += velocities[idx]->vx * dt;
        positions[idx]->y += velocities[idx]->vy * dt;
        positions[idx]->z += velocities[idx]->vz * dt;
    }
}


void ServerGame::broadcast_obstacle_positions() {
    auto &positions = registry_server.get_components<component::position>();
    auto &velocities = registry_server.get_components<component::velocity>();
    
    for (auto obstacleEntity : _obstacles) {
        uint32_t obstacleId = static_cast<uint32_t>(obstacleEntity);
        
        if (obstacleId < positions.size() && positions[obstacleId]) {
            const auto& pos = *positions[obstacleId];
            float vx = 0, vy = 0, vz = 0;
            if (obstacleId < velocities.size() && velocities[obstacleId]) {
                vx = velocities[obstacleId]->vx;
                vy = velocities[obstacleId]->vy;
                vz = velocities[obstacleId]->vz;
            }
            
            broadcast_obstacle_update(obstacleId, pos.x, pos.y, pos.z, vx, vy, vz);
        }
    }
}

void ServerGame::broadcast_obstacle_update(uint32_t obstacleId, float x, float y, float z,
                                          float vx, float vy, float vz) {
    ObstacleUpdateMessage msg{};
    msg.type = MessageType::ObstacleUpdate;
    msg.obstacleId = htonl(obstacleId);

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
