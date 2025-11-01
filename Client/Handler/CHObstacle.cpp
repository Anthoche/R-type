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

void GameClient::handleObstacleSpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ObstacleSpawnMessage)) return;
    const ObstacleSpawnMessage *msg = reinterpret_cast<const ObstacleSpawnMessage *>(buffer.data());
    
    uint32_t id = ntohl(msg->obstacleId);
    uint32_t xb = ntohl(msg->pos.xBits);
    uint32_t yb = ntohl(msg->pos.yBits);
    uint32_t zb = ntohl(msg->pos.zBits);
    uint32_t wb = ntohl(msg->size.widthBits);
    uint32_t hb = ntohl(msg->size.heightBits);
    uint32_t db = ntohl(msg->size.depthBits);
    uint32_t vxb = ntohl(msg->vel.vxBits);
    uint32_t vyb = ntohl(msg->vel.vyBits);
    uint32_t vzb = ntohl(msg->vel.vzBits);

    float x, y, z, w, h, d,  vx, vy, vz;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::memcpy(&z, &zb, sizeof(float));
    std::memcpy(&w, &wb, sizeof(float));
    std::memcpy(&h, &hb, sizeof(float));
    std::memcpy(&d, &db, sizeof(float));
    std::memcpy(&vx, &vxb, sizeof(float));
    std::memcpy(&vy, &vyb, sizeof(float));
    std::memcpy(&vz, &vzb, sizeof(float));

    std::lock_guard<std::mutex> g(stateMutex);
    obstacles[id] = std::make_tuple(x, y, z, w, h, d, vx, vy, vz);
}

void GameClient::handleObstacleUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ObstacleUpdateMessage)) {
        std::cerr << "[ERROR] Invalid ObstacleUpdate message size\n";
        return;
    }

    const ObstacleUpdateMessage *msg = reinterpret_cast<const ObstacleUpdateMessage *>(buffer.data());

    uint32_t id = ntohl(msg->obstacleId);
    uint32_t xb = ntohl(msg->pos.xBits);
    uint32_t yb = ntohl(msg->pos.yBits);
    uint32_t zb = ntohl(msg->pos.zBits);
    uint32_t vxb = ntohl(msg->vel.vxBits);
    uint32_t vyb = ntohl(msg->vel.vyBits);
    uint32_t vzb = ntohl(msg->vel.vzBits);

    float x, y, z, vx, vy, vz;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::memcpy(&z, &zb, sizeof(float));
    std::memcpy(&vx, &vxb, sizeof(float));
    std::memcpy(&vy, &vyb, sizeof(float));
    std::memcpy(&vz, &vzb, sizeof(float));

    std::lock_guard<std::mutex> g(stateMutex);

    auto it = obstacles.find(id);
    if (it != obstacles.end()) {
        float existingWidth = std::get<3>(it->second);
        float existingHeight = std::get<4>(it->second);
        float existingDepth = std::get<5>(it->second);
        obstacles[id] = std::make_tuple(x, y, z, existingWidth, existingHeight, existingDepth, vx, vy, vz);
    }
}

void GameClient::handleObstacleDespawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ObstacleDespawnMessage)) return;
	const ObstacleDespawnMessage *msg = reinterpret_cast<const ObstacleDespawnMessage *>(buffer.data());
	uint32_t id = ntohl(msg->obstacleId);
	std::lock_guard<std::mutex> g(stateMutex);
	obstacles.erase(id);
}