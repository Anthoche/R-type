/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/
#include "Include/server.hpp"
#include "Include/IServerGame.hpp"
#include "../Game/Game_logic/Rtype_game.hpp"
#include <chrono>
#include <thread>
#include <cstring>
#include "Logger.hpp"

GameServer::GameServer(uint16_t port) : connexion(ioContext, port) {
	roomManager = RoomManager(4);
	roomManager.addRoom(Room(4, 2, "Rtype"));
	roomManager.addRoom(Room(4, 4, "Rtype 2"));
	roomManager.addRoom(Room(6, 4, "Test Game"));
	roomManager.addRoom(Room(2, 2, "Test Duo"));
	roomManager.addRoom(Room(12, 10, "Test bonjour")); // Normally, this room should not be created.
}

static void sigintHandler(int s) {
	LOG_INFO("Detected SIGINT signal, shutting down server...");
	exit(1);
}

void GameServer::run() {
	serverRunning = true;

	signal(SIGINT, sigintHandler);
	LOG_INFO("Server started. Now listening for connections...");

	// accept clients until game start
	while (serverRunning) {
		std::vector<uint8_t> data;
		asio::ip::udp::endpoint clientEndpoint;

		// Utilisation d'asyncReceive avec un handler synchrone
		bool received = false;
		connexion.asyncReceive([&](const asio::error_code &error, std::vector<uint8_t> receivedData, asio::ip::udp::endpoint sender) {
			if (!error) {
				data = std::move(receivedData);
				clientEndpoint = sender;
				received = true;
			}
		});

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		if (!received) {
			continue;
		}
		if (data.size() < sizeof(MessageType))
			continue;
		MessageType type = *reinterpret_cast<MessageType *>(data.data());

		switch (type) {
			case MessageType::ClientHello:
				handleClientHello(data, clientEndpoint);
				break;
			case MessageType::ClientRoomIdAsk:
				assignClientToRoom(data, clientEndpoint);
				break;
			case MessageType::ClientFetchRooms:
				handleClientFetchRooms(data, clientEndpoint);
				break;
			default:
				break;
		}
	}
	/*std::cout << "Tous les clients sont connectés. Le jeu commence !" << std::endl;
	// Delegate to Game module
	ServerGame game(connexion);
	game.run();*/
}

void GameServer::handleClientHello(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &clientEndpoint) {
	if (data.size() < sizeof(ClientHelloMessage)) return;
	const ClientHelloMessage *msg = reinterpret_cast<const ClientHelloMessage *>(data.data());

	uint32_t clientId = nextClientId++;
	connexion.addClient(clientEndpoint, clientId);
	uint16_t tcpPort = 5000 + clientId;
	std::thread([this, clientId, tcpPort]() {
		if (!connexion.acceptTcpClient(clientId, tcpPort))
			LOG_ERROR(std::format("TCP accept failed for client {}", clientId));
	}).detach();
	ServerAssignIdMessage assignMsg{};
	assignMsg.type = MessageType::ServerAssignId;
	assignMsg.clientId = htonl(clientId);
	connexion.sendTo(&assignMsg, sizeof(assignMsg), clientEndpoint);
	LOG_INFO(std::format("Client connected: {}({}) [{}/{}]", msg->clientName, clientId, connexion.getClientCount(), maxPlayers));
}

void GameServer::broadcastGameStart() {
	GameStartMessage msg{};
	msg.type = MessageType::GameStart;
	msg.clientCount = htonl(4);
	connexion.broadcast(&msg, sizeof(msg));
	std::cout << "[DEBUG] Message GameStart envoyé à tous les clients." << std::endl;
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

	ServerRoomAssignIdMessage assignMsg{};
	assignMsg.type = MessageType::ServerRoomAssignId;
	assignMsg.roomId = roomId;

	LOG_DEBUG("Sending room assignment");
	connexion.sendTo(&assignMsg, sizeof(assignMsg), from);
	roomManager.getRoom(roomId).addClient(msg->clientId);
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

		r.gameName = room.second.getGame();
		r.minPlayers = room.second.getMinPlayers();
		r.maxPlayers = room.second.getMaxPlayers();
		r.currentPlayers = room.second.getClients().size();
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
	uint32_t xbits = ntohl(msg->inputXBits);
	uint32_t ybits = ntohl(msg->inputYBits);
	float inputX, inputY;
	std::memcpy(&inputX, &xbits, sizeof(float));
	std::memcpy(&inputY, &ybits, sizeof(float));
	auto &pos = playerPositions[id];
	float speed = 200.f / 60.f; // units per tick
	pos.first += inputX * speed;
	pos.second += inputY * speed;
	const float halfSize = 15.f;
	pos.first = std::clamp(pos.first, halfSize, 800.f - halfSize);
	pos.second = std::clamp(pos.second, halfSize, 600.f - halfSize);
}

void GameServer::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point &start, int tick_ms) {
	auto tick_end = std::chrono::high_resolution_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
	if (elapsed_ms < tick_ms)
		std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
}
