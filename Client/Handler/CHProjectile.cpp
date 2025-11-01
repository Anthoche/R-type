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

void GameClient::handleProjectileSpawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ProjectileSpawnMessage)) return;
	const ProjectileSpawnMessage *msg = reinterpret_cast<const ProjectileSpawnMessage *>(buffer.data());

	uint32_t projId = ntohl(msg->projectileId);
	uint32_t ownerId = ntohl(msg->ownerId);
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
	projectiles[projId] = std::make_tuple(x, y, z, vx, vy, vz, ownerId);
}

void GameClient::handleProjectileUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ProjectileUpdateMessage)) return;
    const ProjectileUpdateMessage *msg =
            reinterpret_cast<const ProjectileUpdateMessage *>(buffer.data());

    uint32_t projId = ntohl(msg->projectileId);
    uint32_t xb = ntohl(msg->pos.xBits);
    uint32_t yb = ntohl(msg->pos.yBits);
    uint32_t zb = ntohl(msg->pos.zBits);

    float x, y, z;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::memcpy(&z, &zb, sizeof(float));

    std::lock_guard<std::mutex> g(stateMutex);
    auto it = projectiles.find(projId);
    if (it != projectiles.end()) {
        std::get<0>(it->second) = x;
        std::get<1>(it->second) = y;
        std::get<2>(it->second) = z;
    }
}

void GameClient::handleProjectileDespawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ProjectileDespawnMessage)) return;
	const ProjectileDespawnMessage *msg = reinterpret_cast<const ProjectileDespawnMessage *>(buffer.data());
	uint32_t projId = ntohl(msg->projectileId);

	std::lock_guard<std::mutex> g(stateMutex);
	projectiles.erase(projId);
}

void GameClient::handleEnemyProjectileSpawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(EnemyProjectileSpawnMessage)) return;
	const EnemyProjectileSpawnMessage *msg =
			reinterpret_cast<const EnemyProjectileSpawnMessage *>(buffer.data());

	uint32_t projId = ntohl(msg->projectileId);
	uint32_t ownerId = ntohl(msg->ownerId);
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
	enemyProjectiles[projId] = std::make_tuple(x, y, z, vx, vy, vz, ownerId);
}

void GameClient::handleEnemyProjectileUpdate(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(EnemyProjectileUpdateMessage)) return;
	const EnemyProjectileUpdateMessage *msg =
			reinterpret_cast<const EnemyProjectileUpdateMessage *>(buffer.data());

	uint32_t projId = ntohl(msg->projectileId);
	uint32_t xb = ntohl(msg->pos.xBits);
	uint32_t yb = ntohl(msg->pos.yBits);
	uint32_t zb = ntohl(msg->pos.zBits);

	float x, y, z;
	std::memcpy(&x, &xb, sizeof(float));
	std::memcpy(&y, &yb, sizeof(float));
	std::memcpy(&z, &zb, sizeof(float));

	std::lock_guard<std::mutex> g(stateMutex);
	auto it = enemyProjectiles.find(projId);
	if (it != enemyProjectiles.end()) {
		std::get<0>(it->second) = x;
		std::get<1>(it->second) = y;
		std::get<2>(it->second) = z;
	}
}

void GameClient::handleEnemyProjectileDespawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(EnemyProjectileDespawnMessage)) return;
	const EnemyProjectileDespawnMessage *msg =
			reinterpret_cast<const EnemyProjectileDespawnMessage *>(buffer.data());
	uint32_t projId = ntohl(msg->projectileId);

	std::lock_guard<std::mutex> g(stateMutex);
	enemyProjectiles.erase(projId);
}