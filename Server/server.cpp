/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#include "Include/server.hpp"
#include "Include/IServerGame.hpp"
#include "../Game/Game_logic/Rungame.hpp"
#include <chrono>
#include <thread>
#include <cstring>
#include <string>
#include <optional>
#include <format>
#include "Logger.hpp"
#include "WeaponDefinition.hpp"

GameServer::GameServer(uint16_t port) : connexion(ioContext, port), roomManager(4) {
	roomManager.addRoom(Room(connexion, 4, 4, "Room 1"));
	roomManager.addRoom(Room(connexion, 4, 4, "Room 2"));
	roomManager.addRoom(Room(connexion, 6, 4, "Room 3"));
	roomManager.addRoom(Room(connexion, 2, 2, "Room 4"));
}

static void sigintHandler(int s) {
	LOG_INFO("Detected SIGINT signal, shutting down server...");
	exit(1);
}

void GameServer::run() {
	serverRunning = true;

	signal(SIGINT, sigintHandler);
	LOG_INFO("Server started. Now listening for connections...");
	connexion.startListening();

	while (serverRunning) {
		Connexion::ReceivedPacket packet;
		if (!connexion.waitForPacket(packet, std::chrono::milliseconds(5)))
			continue;
		processIncomingPacket(packet);
		while (connexion.tryPopPacket(packet))
			processIncomingPacket(packet);
	}

	connexion.stopListening();
}

void GameServer::handleClientHello(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &clientEndpoint) {
	if (data.size() < sizeof(ClientHelloMessage)) return;
	const ClientHelloMessage *msg = reinterpret_cast<const ClientHelloMessage *>(data.data());

	uint32_t clientId = nextClientId++;
	connexion.addClient(clientEndpoint, clientId);
	connexion.setClientName(clientId, std::string(msg->clientName));
	uint16_t tcpPort = 5000 + clientId;
	std::thread([this, clientId, tcpPort]() {
		if (!connexion.acceptTcpClient(clientId, tcpPort))
			LOG_ERROR(std::format("TCP accept failed for client {}", clientId));
	}).detach();
	ServerAssignIdMessage assignMsg{};
	assignMsg.type = MessageType::ServerAssignId;
	assignMsg.clientId = htonl(clientId);
	connexion.sendTo(&assignMsg, sizeof(assignMsg), clientEndpoint);

	for (const auto &[otherId, skin]: waitingPlayerSkins) {
		PlayerSkinMessage skinMsg{};
		skinMsg.type = MessageType::PlayerSkinUpdate;
		skinMsg.clientId = htonl(otherId);
		std::memset(skinMsg.skinFilename, 0, sizeof(skinMsg.skinFilename));
		std::strncpy(skinMsg.skinFilename, skin.c_str(), sizeof(skinMsg.skinFilename) - 1);
		connexion.sendTo(&skinMsg, sizeof(skinMsg), clientEndpoint);
	}
	for (const auto &[otherId, weapon] : waitingPlayerWeapons) {
		PlayerWeaponMessage weaponMsg{};
		weaponMsg.type = MessageType::PlayerWeaponUpdate;
		weaponMsg.clientId = htonl(otherId);
		std::memset(weaponMsg.weaponId, 0, sizeof(weaponMsg.weaponId));
		std::strncpy(weaponMsg.weaponId, weapon.c_str(), sizeof(weaponMsg.weaponId) - 1);
		connexion.sendTo(&weaponMsg, sizeof(weaponMsg), clientEndpoint);
	}

	waitingPlayerWeapons[clientId] = "basic_shot";
	PlayerWeaponMessage baseMsg{};
	baseMsg.type = MessageType::PlayerWeaponUpdate;
	baseMsg.clientId = htonl(clientId);
	std::memset(baseMsg.weaponId, 0, sizeof(baseMsg.weaponId));
	std::strncpy(baseMsg.weaponId, waitingPlayerWeapons[clientId].c_str(), sizeof(baseMsg.weaponId) - 1);
	connexion.broadcast(&baseMsg, sizeof(baseMsg));

	LOG_INFO(std::format("Client connected: {}({}) [{}/{}]", msg->clientName, clientId, connexion.getClientCount(), maxPlayers));
}

void GameServer::handlePlayerSkinUpdate(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from) {
	if (data.size() < sizeof(PlayerSkinMessage)) return;
	const PlayerSkinMessage *msg = reinterpret_cast<const PlayerSkinMessage *>(data.data());
	uint32_t clientId = ntohl(msg->clientId);
	std::string filename(msg->skinFilename);
	if (filename.empty()) {
		waitingPlayerSkins.erase(clientId);
	} else {
		waitingPlayerSkins[clientId] = filename;
	}

	PlayerSkinMessage broadcastMsg{};
	broadcastMsg.type = MessageType::PlayerSkinUpdate;
	broadcastMsg.clientId = htonl(clientId);
	std::memset(broadcastMsg.skinFilename, 0, sizeof(broadcastMsg.skinFilename));
	std::strncpy(broadcastMsg.skinFilename, filename.c_str(), sizeof(broadcastMsg.skinFilename) - 1);
	connexion.broadcast(&broadcastMsg, sizeof(broadcastMsg));

	Connexion::ReceivedPacket packet{};
	packet.data = data;
	packet.endpoint = from;
	routePacketToGame(packet, MessageType::PlayerSkinUpdate);
}

void GameServer::handlePlayerWeaponUpdate(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from) {
	if (data.size() < sizeof(PlayerWeaponMessage)) return;
	const PlayerWeaponMessage *msg = reinterpret_cast<const PlayerWeaponMessage*>(data.data());
	uint32_t clientId = ntohl(msg->clientId);
	std::string weaponId(msg->weaponId);
	if (weaponId.empty() || !weapon::hasDefinition(weaponId)) {
		weaponId = "basic_shot";
	}
	waitingPlayerWeapons[clientId] = weaponId;

	PlayerWeaponMessage broadcastMsg{};
	broadcastMsg.type = MessageType::PlayerWeaponUpdate;
	broadcastMsg.clientId = htonl(clientId);
	std::memset(broadcastMsg.weaponId, 0, sizeof(broadcastMsg.weaponId));
	std::strncpy(broadcastMsg.weaponId, weaponId.c_str(), sizeof(broadcastMsg.weaponId) - 1);
	connexion.broadcast(&broadcastMsg, sizeof(broadcastMsg));

	Connexion::ReceivedPacket packet{};
	packet.data.resize(sizeof(broadcastMsg));
	std::memcpy(packet.data.data(), &broadcastMsg, sizeof(broadcastMsg));
	packet.endpoint = from;
	routePacketToGame(packet, MessageType::PlayerWeaponUpdate);
}

void GameServer::broadcastGameStart(uint32_t roomId) {
	auto room = roomManager.getRoom(roomId);

	GameStartMessage msg{};
	msg.type = MessageType::GameStart;
	msg.clientCount = htonl(room->getClients().size());
	connexion.broadcastToRoom(*room.get(), &msg, sizeof(msg));
}

void GameServer::assignClientToRoom(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) {
	if (data.size() < sizeof(ClientRoomIdAskMessage)) return;

	LOG_DEBUG("Received client room ask from client");
	const ClientRoomIdAskMessage *msg = reinterpret_cast<const ClientRoomIdAskMessage *>(data.data());
	int roomId = static_cast<int>(msg->roomId);

	if (!roomManager.roomExists(roomId)) {
		LOG_WARN(std::format("Room with id '{}' does not exists. Skipping.", roomId));
		return;
	}
	auto room = roomManager.getRoom(roomId);

	ServerRoomAssignIdMessage assignMsg{};
	assignMsg.type = MessageType::ServerRoomAssignId;
	assignMsg.roomId = roomId;

	LOG_DEBUG("Sending room assignment");
	connexion.sendTo(&assignMsg, sizeof(assignMsg), from);
	roomManager.addClientInRoom(msg->clientId, roomId);

	if (room->isReady()) {
		RoomReadyMessage roomReadyMsg{};
		roomReadyMsg.type = MessageType::ServerSetRoomReady;
		roomReadyMsg.roomId = roomId;

		LOG_DEBUG("Room is considered ready. Sending room ready message to clients in room.");
		connexion.broadcastToRoom(*room, &roomReadyMsg, sizeof(roomReadyMsg));
	}
	LOG_INFO(std::format("Assigned client {} to room {}", msg->clientId, roomId));
}

void GameServer::handleClientFetchRooms(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) {
	if (data.size() < sizeof(ClientFetchRoomsMessage)) return;

	LOG_DEBUG("Received rooms fetch from client");
	const ClientFetchRoomsMessage *msg = reinterpret_cast<const ClientFetchRoomsMessage *>(data.data());

	auto rooms = roomManager.getRooms();
	std::map<int, game::serializer::RoomData> roomsData;

	for (auto &room: rooms) {
		game::serializer::RoomData r{};

		r.gameName = room.second->getGameName();
		r.minPlayers = room.second->getMinPlayers();
		r.maxPlayers = room.second->getMaxPlayers();
		r.currentPlayers = room.second->getClients().size();
		r.status = GameStatus::PENDING_START;
		roomsData.insert_or_assign(room.first, r);
	}
	auto jsonData = game::serializer::serialize_rooms(roomsData);

	ServerSendRoomsMessage sendMsg;
	sendMsg.type = MessageType::ServerSendRooms;
	strncpy(sendMsg.jsonData, jsonData.dump().c_str(), sizeof(sendMsg.jsonData) - 1);
	sendMsg.jsonData[sizeof(sendMsg.jsonData) - 1] = '\0';
	LOG_DEBUG("Sending JSON with available rooms");
	connexion.sendTo(&sendMsg, sizeof(sendMsg), from);
}

void GameServer::handle_client_message(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) {
	if (data.size() < sizeof(MessageType)) return;
	MessageType type = *reinterpret_cast<const MessageType *>(data.data());
	if (type != MessageType::ClientInput) return;
	if (data.size() < sizeof(ClientInputMessage)) return;
	const ClientInputMessage *msg = reinterpret_cast<const ClientInputMessage *>(data.data());
	uint32_t id = ntohl(msg->clientId);
	InputCode code = static_cast<InputCode>(msg->inputCode);
	bool pressed = msg->isPressed != 0;
	(void) from;
	std::cout << "[ServerBootstrap][Input] received event from client " << id
			<< " code=" << inputCodeToString(code)
			<< " pressed=" << pressed << std::endl;
}

void GameServer::handleClientConfirmStart(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) {
	if (data.size() < sizeof(ClientConfirmStartMessage)) return;

	const ClientConfirmStartMessage *msg = reinterpret_cast<const ClientConfirmStartMessage *>(data.data());
	if (!roomManager.roomExists(msg->roomId)) return;

	auto room = roomManager.getRoom(msg->roomId);

	if (!room->isReady()) return;

	room->setClientConfirmed(msg->clientId);
	for (auto c: room->getClients()) {
		if (!c.second)
			return;
	}
	roomManager.startRoom(msg->roomId);
	broadcastGameStart(msg->roomId);
}

void GameServer::handleClientLeaveRoom(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) {
	if (data.size() < sizeof(ClientLeaveRoomMessage)) return;

	const ClientLeaveRoomMessage *msg = reinterpret_cast<const ClientLeaveRoomMessage *>(data.data());
	if (!roomManager.roomExists(msg->roomId)) return;

	auto room = roomManager.getRoom(msg->roomId);
	room->removeClient(msg->clientId);
}

void GameServer::processIncomingPacket(const Connexion::ReceivedPacket &packet) {
	if (packet.data.size() < sizeof(MessageType))
		return;
	MessageType type = *reinterpret_cast<const MessageType *>(packet.data.data());

	switch (type) {
		case MessageType::ClientHello:
			handleClientHello(packet.data, packet.endpoint);
			break;
		case MessageType::ClientRoomIdAsk:
			assignClientToRoom(packet.data, packet.endpoint);
			break;
		case MessageType::ClientFetchRooms:
			handleClientFetchRooms(packet.data, packet.endpoint);
			break;
		case MessageType::PlayerSkinUpdate:
			handlePlayerSkinUpdate(packet.data, packet.endpoint);
			break;
		case MessageType::PlayerWeaponUpdate:
			handlePlayerWeaponUpdate(packet.data, packet.endpoint);
		case MessageType::ClientConfirmStart:
			handleClientConfirmStart(packet.data, packet.endpoint);
			break;
		case MessageType::ClientLeaveRoom:
			handleClientLeaveRoom(packet.data, packet.endpoint);
			break;
		default:
			routePacketToGame(packet, type);
			break;
	}
}

void GameServer::routePacketToGame(const Connexion::ReceivedPacket &packet, MessageType type) {
	uint32_t clientId = 0;
	bool hasClient = false;

	switch (type) {
		case MessageType::ClientInput:
			if (packet.data.size() >= sizeof(ClientInputMessage)) {
				const auto *msg = reinterpret_cast<const ClientInputMessage *>(packet.data.data());
				clientId = ntohl(msg->clientId);
				hasClient = true;
			}
			break;
		case MessageType::ChatMessage:
			if (packet.data.size() >= sizeof(ChatMessagePacket)) {
				const auto *msg = reinterpret_cast<const ChatMessagePacket *>(packet.data.data());
				clientId = ntohl(msg->senderId);
				hasClient = true;
			}
			break;
		case MessageType::SceneState:
			if (packet.data.size() >= sizeof(SceneStateMessage)) {
				const auto *msg = reinterpret_cast<const SceneStateMessage *>(packet.data.data());
				clientId = ntohl(msg->clientId);
				hasClient = true;
			}
			break;
		case MessageType::ClientShoot:
			if (packet.data.size() >= sizeof(ClientShootMessage)) {
				const auto *msg = reinterpret_cast<const ClientShootMessage *>(packet.data.data());
				clientId = ntohl(msg->clientId);
				hasClient = true;
			}
			break;
		case MessageType::PlayerSkinUpdate:
			if (packet.data.size() >= sizeof(PlayerSkinMessage)) {
				const auto *msg = reinterpret_cast<const PlayerSkinMessage *>(packet.data.data());
				clientId = ntohl(msg->clientId);
				hasClient = true;
			}
			break;
		case MessageType::PlayerWeaponUpdate:
			if (packet.data.size() >= sizeof(PlayerWeaponMessage)) {
				const auto *msg = reinterpret_cast<const PlayerWeaponMessage *>(packet.data.data());
				clientId = ntohl(msg->clientId);
				hasClient = true;
			}
			break;
		case MessageType::InitialHealth:
			if (packet.data.size() >= sizeof(InitialHealthMessage)) {
				const auto *msg = reinterpret_cast<const InitialHealthMessage *>(packet.data.data());
				clientId = ntohl(msg->clientId);
				hasClient = true;
			}
			break;
		default:
			break;
	}

	if (!hasClient)
		return;

	auto roomId = roomManager.findRoomIdByClient(clientId);
	if (!roomId.has_value()) {
		LOG_DEBUG(std::format("Dropping message {} from client {}: client not assigned to a room.", static_cast<int>(type), clientId));
		return;
	}

	auto room = roomManager.getRoom(roomId.value());
	if (!room) {
		LOG_WARN(std::format("routePacketToGame(): room {} not found while handling client {}", roomId.value(), clientId));
		return;
	}

	auto game = room->getGame();
	if (!game) {
		LOG_WARN(std::format("routePacketToGame(): game instance missing for room {}", roomId.value()));
		return;
	}
	game->enqueuePacket(packet.data, packet.endpoint);
}

void GameServer::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point &start, int tick_ms) {
	auto tick_end = std::chrono::high_resolution_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
	if (elapsed_ms < tick_ms)
		std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
}
