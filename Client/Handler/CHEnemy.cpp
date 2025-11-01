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

void GameClient::handleEnemySpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(EnemySpawnMessage)) return;
    const EnemySpawnMessage *msg = reinterpret_cast<const EnemySpawnMessage *>(buffer.data());
    
    uint32_t enemyId = ntohl(msg->enemyId);
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
    enemies[enemyId] = std::make_tuple(x, y, z, vx, vy, vz, bw, bh);
}

void GameClient::handleEnemyUpdate(const std::vector<uint8_t>& data) {
    if (data.size() >= sizeof(EnemyUpdateMessage)) {
        const EnemyUpdateMessage* msg = reinterpret_cast<const EnemyUpdateMessage*>(data.data());

        uint32_t enemyId = ntohl(msg->enemyId);


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
        auto it = enemies.find(enemyId);
        float existingWidth = 0.0f;
        float existingHeight = 0.0f;
        if (it != enemies.end()) {
            vz = std::get<5>(it->second);
            existingWidth = std::get<6>(it->second);
            existingHeight = std::get<7>(it->second);
        }
        enemies[enemyId] = std::make_tuple(x, y, z, vx, vy, vz, existingWidth, existingHeight);
    }
}


void GameClient::handleEnemyDespawn(const std::vector<uint8_t> &data) {
	if (data.size() >= sizeof(EnemyDespawnMessage)) {
		const EnemyDespawnMessage *msg = reinterpret_cast<const EnemyDespawnMessage *>(data.data());

		uint32_t enemyId = ntohl(msg->enemyId);

		std::lock_guard<std::mutex> lock(stateMutex);
		enemies.erase(enemyId);
	}
}

void GameClient::handleBossDeath(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(BossDeathMessage)) {
        return;
    }
    BossDeathMessage msg;
    std::memcpy(&msg, buffer.data(), sizeof(BossDeathMessage));
    uint32_t bossId = ntohl(msg.bossId);
    bossDefeated = true;
    _lastBoss = ntohl(msg._lastBoss);
    fetchFullRegistryAsync();
}
