/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** client_handler
*/

#include "../client.hpp"
#include "Logger.hpp"
#include "../../Engine/Game.hpp"
#include <asio.hpp>
#include <cstring>
#include <algorithm>

void GameClient::handleElementSpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ElementSpawnMessage)) return;
    const ElementSpawnMessage *msg = reinterpret_cast<const ElementSpawnMessage *>(buffer.data());
    
    uint32_t elementId = ntohl(msg->elementId);
    uint32_t xb = ntohl(msg->pos.xBits);
    uint32_t yb = ntohl(msg->pos.yBits);
    uint32_t zb = ntohl(msg->pos.zBits);
    uint32_t vxb = ntohl(msg->vel.vxBits);
    uint32_t vyb = ntohl(msg->vel.vyBits);
    uint32_t vzb = ntohl(msg->vel.vzBits);
    uint32_t w = ntohl(msg->width);
    uint32_t h = ntohl(msg->height);
    
    float x, y, z, vx, vy, vz, bw, bh;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::memcpy(&z, &zb, sizeof(float));
    std::memcpy(&vx, &vxb, sizeof(float));
    std::memcpy(&vy, &vyb, sizeof(float));
    std::memcpy(&vz, &vzb, sizeof(float));
    std::memcpy(&bw, &w, sizeof(float));
    std::memcpy(&bh, &h, sizeof(float));
    
    std::lock_guard<std::mutex> g(stateMutex);
    elements[elementId] = std::make_tuple(x, y, z, vx, vy, vz, bw, bh);
}

void GameClient::handleElementUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() >= sizeof(ElementUpdateMessage)) {
        const ElementUpdateMessage* msg = reinterpret_cast<const ElementUpdateMessage*>(buffer.data());
        
        uint32_t elementId = ntohl(msg->elementId);
        
        
        uint32_t xb = ntohl(msg->pos.xBits);
        uint32_t yb = ntohl(msg->pos.yBits);
        uint32_t zb = ntohl(msg->pos.zBits);
        uint32_t vxb = ntohl(msg->velXBits);
        uint32_t vyb = ntohl(msg->velYBits);
    
        float x, y, z, vx, vy;
        std::memcpy(&x, &xb, sizeof(float));
        std::memcpy(&y, &yb, sizeof(float));
        std::memcpy(&z, &zb, sizeof(float));
        std::memcpy(&vx, &vxb, sizeof(float));
        std::memcpy(&vy, &vyb, sizeof(float));
    
        std::lock_guard<std::mutex> lock(stateMutex);
        float vz = 0.f;
        auto it = elements.find(elementId);
        float existingWidth = 0.0f;
        float existingHeight = 0.0f;
        if (it != elements.end()) {
            vz = std::get<5>(it->second);
            existingWidth = std::get<6>(it->second);
            existingHeight = std::get<7>(it->second);
        }
        elements[elementId] = std::make_tuple(x, y, z, vx, vy, vz, existingWidth, existingHeight);
    }
}

void GameClient::handleElementDespawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() >= sizeof(ElementDespawnMessage)) {
        const ElementDespawnMessage* msg = reinterpret_cast<const ElementDespawnMessage*>(buffer.data());
        
        uint32_t elementId = ntohl(msg->elementId);
        
        std::lock_guard<std::mutex> lock(stateMutex);
        elements.erase(elementId);
    }
}
