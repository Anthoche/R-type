/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#pragma once

#include "connexion.hpp"
#include "IServerGame.hpp"
#include "../Shared/protocol.hpp"
#include <asio.hpp>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>
#include "../Room/RoomManager.hpp"
#include <string>

/**
 * @class GameServer
 * @brief Main server class that manages the R-Type multiplayer game backend.
 *
 * The GameServer handles all server-side operations including:
 * - Client connection management and authentication
 * - Room creation and player assignment
 * - Game instance lifecycle management
 * - Network message routing between clients and game instances
 * - Player customization (skins and weapons) synchronization
 * - Lobby state management (waiting for players, ready checks)
 *
 * The server supports multiple concurrent game rooms, each running independent
 * game instances with their own physics simulation and state management.
 */
class GameServer {
		int maxPlayers = 10; ///< Maximum number of players per game room.
		asio::io_context ioContext; ///< ASIO I/O context for asynchronous network operations.
		Connexion connexion; ///< Network connection manager handling UDP and TCP communications.
		bool serverRunning{false}; ///< Flag indicating if the server is currently running.
		uint32_t nextClientId = 1; ///< Counter for assigning unique client IDs.
		std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Maps player IDs to their (x, y) positions.
		uint32_t nextEnemyId = 1; ///< Counter for assigning unique enemy IDs.
		std::unordered_map<uint32_t, std::pair<float, float>> enemies; ///< Maps enemy IDs to their (x, y) positions.
		float enemySpawnTimerSec = 0.f; ///< Timer tracking when to spawn the next enemy.
		std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles; ///< Maps obstacle IDs to (x, y, width, height).
		std::unordered_map<uint32_t, std::string> waitingPlayerSkins; ///< Stores skin selections for players in lobby.
		std::unordered_map<uint32_t, std::string> waitingPlayerWeapons; ///< Stores weapon selections for players in lobby.
		RoomManager roomManager; ///< Manages all game rooms and their states.

	public:
		/**
		 * @brief Constructs a GameServer instance and initializes networking.
		 *
		 * Binds the UDP socket to the specified port and prepares the server
		 * to accept incoming client connections.
		 *
		 * @param port UDP port number to listen on for client connections.
		 */
		GameServer(uint16_t port);

		/**
		 * @brief Starts the main server loop.
		 *
		 * Runs continuously to process incoming packets, manage client connections,
		 * handle room creation and assignment, and coordinate game instance lifecycle.
		 * This function blocks until the server is shut down.
		 */
		void run();

		/**
		 * @brief Destructor that cleans up server resources.
		 */
		~GameServer() = default;

	private:
		/**
		 * @brief Handles a ClientHello message from a new client.
		 *
		 * Processes initial connection requests, assigns a unique client ID,
		 * and sends acknowledgment back to the client.
		 *
		 * @param data Raw message data containing client information.
		 * @param clientEndpoint UDP endpoint (address and port) of the connecting client.
		 */
		void handleClientHello(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &clientEndpoint);

		/**
		 * @brief Handles a ClientRoomIdAsk message requesting room assignment.
		 *
		 * Processes a client's request to join a specific room, validates the request,
		 * and assigns the client to the room if possible.
		 *
		 * @param data Raw message data containing the requested room ID.
		 * @param from UDP endpoint of the requesting client.
		 */
		void assignClientToRoom(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);

		/**
		 * @brief Handles a ClientFetchRooms message requesting the list of available rooms.
		 *
		 * Sends the current list of game rooms to the requesting client,
		 * including room status, player counts, and game settings.
		 *
		 * @param data Raw message data.
		 * @param from UDP endpoint of the requesting client.
		 */
		void handleClientFetchRooms(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);

		/**
		 * @brief Handles a ClientConfirmStart message indicating a player is ready.
		 *
		 * Marks the client as ready to start the game. When all players in a room
		 * are ready, the game begins.
		 *
		 * @param data Raw message data.
		 * @param from UDP endpoint of the client confirming ready status.
		 */
		void handleClientConfirmStart(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);

		/**
		 * @brief Handles a ClientLeaveRoom message when a player leaves a room.
		 *
		 * Removes the client from their current room and notifies other players.
		 *
		 * @param data Raw message data.
		 * @param from UDP endpoint of the leaving client.
		 */
		void handleClientLeaveRoom(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);

		/**
		 * @brief Broadcasts a GameStart message to all players in a room.
		 *
		 * Signals the beginning of gameplay for all clients in the specified room.
		 *
		 * @param roomId The unique identifier of the room starting the game.
		 */
		void broadcastGameStart(uint32_t roomId);

		/**
		 * @brief Processes a received network packet.
		 *
		 * Determines the message type and routes the packet to the appropriate handler.
		 *
		 * @param packet The received packet containing data and sender information.
		 */
		void processIncomingPacket(const Connexion::ReceivedPacket &packet);

		/**
		 * @brief Routes a gameplay packet to the appropriate game instance.
		 *
		 * Forwards packets intended for active game instances (e.g., player input,
		 * shoot commands) to the correct running game.
		 *
		 * @param packet The received packet to route.
		 * @param type The type of message contained in the packet.
		 */
		void routePacketToGame(const Connexion::ReceivedPacket &packet, MessageType type);

		/**
		 * @brief Handles client messages during active gameplay.
		 *
		 * Processes player input, actions, and other gameplay-related messages.
		 *
		 * @param data Raw message data.
		 * @param from UDP endpoint of the sending client.
		 */
		void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

		/**
		 * @brief Handles a PlayerSkinUpdate message from a client.
		 *
		 * Updates the server's record of the player's selected skin and broadcasts
		 * the change to other players in the same room.
		 *
		 * @param data Raw message data containing the skin selection.
		 * @param from UDP endpoint of the client updating their skin.
		 */
		void handlePlayerSkinUpdate(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

		/**
		 * @brief Handles a PlayerWeaponUpdate message from a client.
		 *
		 * Updates the server's record of the player's selected weapon and broadcasts
		 * the change to other players in the same room.
		 *
		 * @param data Raw message data containing the weapon selection.
		 * @param from UDP endpoint of the client updating their weapon.
		 */
		void handlePlayerWeaponUpdate(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

		/**
		 * @brief Maintains a consistent server tick rate by sleeping.
		 *
		 * Calculates the remaining time in the current tick and sleeps to ensure
		 * the server runs at a fixed update rate.
		 *
		 * @param start Timestamp when the current tick began.
		 * @param tick_ms Desired duration of each tick in milliseconds.
		 */
		void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point &start, int tick_ms);
};