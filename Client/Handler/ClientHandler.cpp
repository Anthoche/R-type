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

void GameClient::handleMessage(MessageType type, const std::vector<uint8_t> &buffer) {
	switch (type) {
		case MessageType::ServerAssignId:
			handleServerAssignId(buffer);
			break;
		case MessageType::ServerRoomAssignId:
			handleServerRoomAssign(buffer);
			break;
		case MessageType::ServerSendRooms:
			handleServerRooms(buffer);
			break;
		case MessageType::GameStart:
			handleGameStart(buffer);
			break;
		case MessageType::ServerSetRoomReady:
			handleRoomReady(buffer);
			break;
		case MessageType::ServerSetClientConfirmed:
			LOG_DEBUG("Received ServerSetClientConfirmed message");
			break;
		case MessageType::StateUpdate:
			handlePlayerUpdate(buffer);
			break;
		case MessageType::ObstacleSpawn:
			handleObstacleSpawn(buffer);
			break;
		case MessageType::ObstacleUpdate:
			handleObstacleUpdate(buffer);
			break;
		case MessageType::ObstacleDespawn:
			handleObstacleDespawn(buffer);
			break;
		case MessageType::PlayerHealth:
			handlePlayerHealth(buffer);
			break;
		case MessageType::GlobalScore:
			handleGlobalScore(buffer);
			break;
		case MessageType::IndividualScore:
			handleIndividualScore(buffer);
			break;
		case MessageType::ProjectileSpawn:
			handleProjectileSpawn(buffer);
			break;
		case MessageType::ProjectileDespawn:
			handleProjectileDespawn(buffer);
			break;
		case MessageType::ProjectileUpdate:
			handleProjectileUpdate(buffer);
			break;
		case MessageType::EnemySpawn:
			handleEnemySpawn(buffer);
			break;
		case MessageType::EnemyDespawn:
			handleEnemyDespawn(buffer);
			break;
		case MessageType::BossDeath:
			handleBossDeath(buffer);
			break;
		case MessageType::EnemyUpdate:
			handleEnemyUpdate(buffer);
			break;
    case MessageType::EndlessMode:
      handleEndlessMode(buffer);
      break;
		case MessageType::PlayerDeath:
			handlePlayerDeath(buffer);
			break;
		case MessageType::EnemyProjectileSpawn:
			handleEnemyProjectileSpawn(buffer);
			break;
		case MessageType::EnemyProjectileUpdate:
			handleEnemyProjectileUpdate(buffer);
			break;
		case MessageType::EnemyProjectileDespawn:
			handleEnemyProjectileDespawn(buffer);
			break;
		case MessageType::PlayerSkinUpdate:
			handlePlayerSkinUpdate(buffer);
			break;
		case MessageType::PlayerWeaponUpdate:
			handlePlayerWeaponUpdate(buffer);
			break;
		case MessageType::ChatMessage:
			handleChatMessage(buffer);
			break;
    case MessageType::InitialHealth:
        handleInitialHealth(buffer);
        break;
		default:
			break;
	}
}

void GameClient::handleServerAssignId(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ServerAssignIdMessage)) return;
    const ServerAssignIdMessage *msg = reinterpret_cast<const ServerAssignIdMessage *>(buffer.data());
    clientId = ntohl(msg->clientId);
    std::cout << "[Client] Reçu clientId=" << clientId << std::endl;
    initTcpConnection();
    std::string pendingSkin;
    std::string pendingWeapon;
    {
        std::lock_guard<std::mutex> g(stateMutex);
        pendingSkin = pendingSkinSelection;
        pendingWeapon = pendingWeaponSelection;
    }
    if (!pendingSkin.empty()) {
        sendSkinSelection(pendingSkin);
    }
    if (!pendingWeapon.empty()) {
        sendWeaponSelection(pendingWeapon);
    }
}

void GameClient::handleServerRoomAssign(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ServerRoomAssignIdMessage)) return;
	const ServerRoomAssignIdMessage *msg = reinterpret_cast<const ServerRoomAssignIdMessage *>(buffer.data());
	roomId = static_cast<int>(msg->roomId);
	LOG_INFO(std::format("Client assigned to room {}", roomId));
}

void GameClient::handleServerRooms(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ServerSendRoomsMessage)) return;
	const ServerSendRoomsMessage *msg = reinterpret_cast<const ServerSendRoomsMessage *>(buffer.data());
	LOG_INFO("Server sent rooms");
	LOG_DEBUG(std::format("Received JSON:\n{}\n", msg->jsonData));
	try {
		nlohmann::json json = nlohmann::json::parse(msg->jsonData);
		auto parsedRooms = game::serializer::deserialize_rooms(json);
		{
			std::lock_guard<std::mutex> lock(roomsMutex);
			rooms = std::move(parsedRooms);
			roomsUpdated = true;
		}
		roomsCv.notify_all();
	} catch (const std::exception &e) {
		LOG_ERROR(std::format("Failed to deserialize rooms JSON: {}", e.what()));
	}
}

void GameClient::handleRoomReady(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(RoomReadyMessage)) return;
	const RoomReadyMessage *msg = reinterpret_cast<const RoomReadyMessage *>(buffer.data());

	if (msg->roomId != roomId)
		return;
	_game.setGameStatus(GameStatus::PENDING_START);
}

void GameClient::handleGameStart(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(GameStartMessage)) return;
	const GameStartMessage *msg = reinterpret_cast<const GameStartMessage *>(buffer.data());
	std::cout << "Le jeu commence ! Nombre de joueurs : " << ntohl(msg->clientCount) << std::endl;
	_game.setGameStatus(GameStatus::RUNNING);
}

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
	players[id] = {x, y, z}; // Adapter selon votre structure
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
    } else {
        std::cerr << "[WARNING] Received update for unknown obstacle: " << id << std::endl;
    }
}

void GameClient::handleObstacleDespawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ObstacleDespawnMessage)) return;
	const ObstacleDespawnMessage *msg = reinterpret_cast<const ObstacleDespawnMessage *>(buffer.data());
	uint32_t id = ntohl(msg->obstacleId);
	std::lock_guard<std::mutex> g(stateMutex);
	obstacles.erase(id);
}

void GameClient::handleEndlessMode(const std::vector<uint8_t> &buffer) {
    if (buffer.size() >= sizeof(EndlessModeMessage)) {
        const EndlessModeMessage* msg = reinterpret_cast<const EndlessModeMessage*>(buffer.data());
        bool isEndless = msg->isEndless != 0;
        _game.setEndlessMode(isEndless);
    }
}

void GameClient::handleInitialHealth(const std::vector<uint8_t> &buffer) {
    if (buffer.size() >= sizeof(InitialHealthMessage)) {
        const InitialHealthMessage* msg = reinterpret_cast<const InitialHealthMessage*>(buffer.data());
        uint32_t senderId = ntohl(msg->clientId);
        size_t health = ntohs(msg->initialHealth);
        _game.setHealth(health);
    }
}

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

void GameClient::handleProjectileDespawn(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(ProjectileDespawnMessage)) return;
	const ProjectileDespawnMessage *msg = reinterpret_cast<const ProjectileDespawnMessage *>(buffer.data());
	uint32_t projId = ntohl(msg->projectileId);

	std::lock_guard<std::mutex> g(stateMutex);
	projectiles.erase(projId);
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
    
    std::cout << "[Client] Enemy spawned: id=" << enemyId 
              << " pos=(" << x << ", " << y << ", " << z << ")" << std::endl;
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

void GameClient::handlePlayerHealth(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(PlayerHealthMessage)) return;
	const PlayerHealthMessage *msg = reinterpret_cast<const PlayerHealthMessage *>(buffer.data());

	uint32_t playerId = ntohl(msg->clientId);
	int16_t currentHealth = ntohs(msg->currentHealth);
	int16_t maxHealth = ntohs(msg->maxHealth);

	std::lock_guard<std::mutex> g(stateMutex);

	playerHealth[playerId] = {currentHealth, maxHealth};
}

void GameClient::handleGlobalScore(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(GlobalScoreMessage)) return;
	const GlobalScoreMessage *msg = reinterpret_cast<const GlobalScoreMessage *>(buffer.data());

	int32_t globalScore = ntohl(msg->totalScore);

	std::lock_guard<std::mutex> g(stateMutex);
	this->globalScore = globalScore;
}

void GameClient::handleIndividualScore(const std::vector<uint8_t> &buffer) {
	if (buffer.size() < sizeof(IndividualScoreMessage)) return;
	const IndividualScoreMessage *msg = reinterpret_cast<const IndividualScoreMessage *>(buffer.data());

	uint32_t playerId = ntohl(msg->clientId);
	uint32_t score = ntohl(msg->score);

	std::lock_guard<std::mutex> g(stateMutex);
	playerIndividualScores[playerId] = score;
}

void GameClient::handleChatMessage(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ChatMessagePacket))
        return;
    const ChatMessagePacket *msg = reinterpret_cast<const ChatMessagePacket *>(buffer.data());

    auto senderEnd = std::find(std::begin(msg->senderName), std::end(msg->senderName), '\0');
    auto messageEnd = std::find(std::begin(msg->message), std::end(msg->message), '\0');
    std::string sender(msg->senderName, static_cast<std::size_t>(senderEnd - std::begin(msg->senderName)));
    std::string text(msg->message, static_cast<std::size_t>(messageEnd - std::begin(msg->message)));

    if (text.empty())
        return;

    {
        std::lock_guard<std::mutex> g(stateMutex);
        _chatQueue.emplace_back(std::move(sender), std::move(text));
    }
}
