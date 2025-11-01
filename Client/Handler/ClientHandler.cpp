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
        case MessageType::ServerSetRoomReady:
            handleRoomReady(buffer);
            break;
        case MessageType::GameStart:
            handleGameStart(buffer);
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
        case MessageType::EndlessMode:
            handleEndlessMode(buffer);
            break;
        case MessageType::InitialHealth:
            handleInitialHealth(buffer);
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
        case MessageType::ElementSpawn:
            handleElementSpawn(buffer);
            break;
        case MessageType::ElementUpdate:
            handleElementUpdate(buffer);
            break;
        case MessageType::ElementDespawn:
            handleElementDespawn(buffer);
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
