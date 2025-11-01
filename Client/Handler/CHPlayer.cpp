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

void GameClient::handlePlayerUpdate(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(StateUpdateMessage)) return;
	const StateUpdateMessage *msg = reinterpret_cast<const StateUpdateMessage *>(buffer.data());

	uint32_t id = ntohl(msg->clientId);
	uint32_t xb = ntohl(msg->pos.xBits);
	uint32_t yb = ntohl(msg->pos.yBits);
	uint32_t zb = ntohl(msg->pos.zBits);

	float x, y, z;
	std::memcpy(&x, &xb, sizeof(float));
	std::memcpy(&y, &yb, sizeof(float));
	std::memcpy(&z, &zb, sizeof(float));

	std::lock_guard<std::mutex> g(stateMutex);
	players[id] = {x, y, z};
}

void GameClient::handlePlayerSkinUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(PlayerSkinMessage)) return;
    const PlayerSkinMessage *msg = reinterpret_cast<const PlayerSkinMessage *>(buffer.data());

    uint32_t id = ntohl(msg->clientId);
    std::string filename(msg->skinFilename);

    std::lock_guard<std::mutex> g(stateMutex);
    if (!filename.empty()) {
        playerSkins[id] = filename;
    } else {
        playerSkins.erase(id);
    }
}

void GameClient::handlePlayerWeaponUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(PlayerWeaponMessage)) return;
    const PlayerWeaponMessage *msg = reinterpret_cast<const PlayerWeaponMessage *>(buffer.data());

    uint32_t id = ntohl(msg->clientId);
    std::string weaponId(msg->weaponId);

    std::lock_guard<std::mutex> g(stateMutex);
    if (!weaponId.empty()) {
        playerWeapons[id] = weaponId;
    } else {
        playerWeapons.erase(id);
    }
}

void GameClient::handlePlayerHealth(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(PlayerHealthMessage)) return;
	const PlayerHealthMessage *msg = reinterpret_cast<const PlayerHealthMessage *>(buffer.data());

	uint32_t playerId = ntohl(msg->clientId);
	int16_t currentHealth = ntohs(msg->currentHealth);
	int16_t maxHealth = ntohs(msg->maxHealth);

	std::lock_guard<std::mutex> g(stateMutex);

	playerHealth[playerId] = {currentHealth, maxHealth};
}

void GameClient::handlePlayerDeath(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(PlayerDeathMessage)) return;
	const PlayerDeathMessage *msg = reinterpret_cast<const PlayerDeathMessage *>(buffer.data());
	uint32_t deadPlayerId = ntohl(msg->clientId);

	std::lock_guard<std::mutex> g(stateMutex);
	players.erase(deadPlayerId);

	std::cout << "[Client] Player " << deadPlayerId << " died!" << std::endl;

	if (deadPlayerId == clientId) {
		std::cout << "[Client] YOU DIED!" << std::endl;
	}
}
