/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGEnnemy
*/


#include "../Rungame.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <cmath>
#include <chrono>
#include "../../../Shared/Logger.hpp"


template <typename Component>
inline Component* get_component_ptr(ecs::registry &registry, ecs::entity_t entity) {
    auto &arr = registry.get_components<Component>();
    auto idx = static_cast<std::size_t>(entity);
    if (idx >= arr.size() || !arr[idx].has_value()) {
        return nullptr;
    }
    return &arr[idx].value();
}

void ServerGame::broadcast_element_spawn(uint32_t enemyId, float x, float y, float z, float vx, float vy, float vz, float width, float height) {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    ElementSpawnMessage msg{};
    msg.type = MessageType::ElementSpawn;
    msg.elementId = htonl(enemyId);

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

    connexion.broadcastToClients(recipients, &msg, sizeof(msg));
}

void ServerGame::broadcast_element_positions() {
auto &positions = registry_server.get_components<component::position>();
    
    for (auto elementEntity : _randomElements) {
        uint32_t elementId = static_cast<uint32_t>(elementEntity);
        
        if (elementId < positions.size() && positions[elementId]) {
            const auto& pos = *positions[elementId];
            broadcast_element_update(elementId, pos.x, pos.y, pos.z);
        }
    }
}

void ServerGame::update_element(float dt) {
    auto &positions = registry_server.get_components<component::position>();
    auto &velocities = registry_server.get_components<component::velocity>();
    
    for (auto element : _randomElements) {
        uint32_t idx = static_cast<uint32_t>(element);
        
        if (idx >= positions.size() || !positions[idx]) continue;
        if (idx >= velocities.size() || !velocities[idx]) continue;
        
        positions[idx]->x += velocities[idx]->vx * dt;
        positions[idx]->y += velocities[idx]->vy * dt;
        positions[idx]->z += velocities[idx]->vz * dt;
    }
}

void ServerGame::broadcast_element_update(uint32_t elementId, float x, float y, float z) {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    ElementUpdateMessage msg{};
    msg.type = MessageType::ElementUpdate;
    msg.elementId = htonl(elementId);

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
    if (elementId < velocities.size() && velocities[elementId]) {
        float vx = velocities[elementId]->vx;
        float vy = velocities[elementId]->vy;
        std::memcpy(&vxb, &vx, sizeof(float));
        std::memcpy(&vyb, &vy, sizeof(float));
    }

    msg.velXBits = htonl(vxb);
    msg.velYBits = htonl(vyb);

    connexion.broadcastToClients(recipients, &msg, sizeof(msg));
}

void ServerGame::check_player_element_collisions() {
    auto &positions = registry_server.get_components<component::position>();
    auto &collisionBoxes = registry_server.get_components<component::collision_box>();
    auto &types = registry_server.get_components<component::type>();
    auto &healths = registry_server.get_components<component::health>();
    auto &clientIds = registry_server.get_components<component::client_id>();

    std::vector<uint32_t> elementsToRemove;

    for (const auto& [playerId, playerPosPair] : playerPositions) {
        float playerX = playerPosPair.first;
        float playerY = playerPosPair.second;
        const float PLAYER_WIDTH = 30.f;
        const float PLAYER_HEIGHT = 30.f;

        for (auto element : _randomElements) {
            uint32_t elemId = static_cast<uint32_t>(element);
            if (elemId >= positions.size() || !positions[elemId] ||
                elemId >= collisionBoxes.size() || !collisionBoxes[elemId] ||
                elemId >= types.size() || !types[elemId])
                continue;

            if (types[elemId]->value != component::entity_type::RANDOM_ELEMENT)
                continue;

            float elemX = positions[elemId]->x;
            float elemY = positions[elemId]->y;
            float elemW = collisionBoxes[elemId]->width;
            float elemH = collisionBoxes[elemId]->height;

            float playerLeft = playerX - PLAYER_WIDTH / 2;
            float playerRight = playerX + PLAYER_WIDTH / 2;
            float playerTop = playerY - PLAYER_HEIGHT / 2;
            float playerBottom = playerY + PLAYER_HEIGHT / 2;
            
            float elemLeft = elemX - elemW / 2;
            float elemRight = elemX + elemW / 2;
            float elemTop = elemY - elemH / 2;
            float elemBottom = elemY + elemH / 2;

            if (check_aabb_overlap(playerLeft, playerRight, playerTop, playerBottom,
                                   elemLeft, elemRight, elemTop, elemBottom)) {
                LOG_INFO("[Server] Player " << playerId << " collected element " << elemId);                
                for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
                    if (clientIds[i] && clientIds[i]->id == playerId && healths[i]) {
                        healths[i]->current = std::min(healths[i]->current + 25, healths[i]->max);
                        LOG_INFO("[Server] Player " << playerId << " healed to " 
                                << healths[i]->current << "/" << healths[i]->max);
                        break;
                    }
                }
                
                elementsToRemove.push_back(elemId);
            }
        }
    }    
    for (uint32_t elemId : elementsToRemove) {
        registry_server.kill_entity(static_cast<ecs::entity_t>(elemId));
        _randomElements.erase(
            std::remove(_randomElements.begin(), _randomElements.end(), 
                       static_cast<ecs::entity_t>(elemId)),
            _randomElements.end()
        );
        broadcast_element_despawn(elemId);
    }
}
void ServerGame::broadcast_element_despawn(uint32_t elemId) {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    ElementDespawnMessage msg{};
    msg.type = MessageType::ElementDespawn;
    msg.elementId = htonl(elemId);

    connexion.broadcastToClients(recipients, &msg, sizeof(msg));
}
