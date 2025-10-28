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
 * @brief Main server class for the R-Type multiplayer game.
 *
 * Manages client connections, network communication, and game state.
 * Supports up to 4 players and handles:
 * - Receiving client messages (UDP/TCP)
 * - Broadcasting game start messages
 * - Player positions and movement
 * - Enemy and obstacle management
 */
class GameServer {
    int maxPlayers = 10;
    asio::io_context ioContext; ///< ASIO I/O context for networking
    Connexion connexion; ///< Network connection manager (UDP + TCP)
	bool serverRunning{false}; ///< Flag indicating if the game has started
    uint32_t nextClientId = 1; ///< Next available client ID
    std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Maps player IDs to positions (x,y)
    uint32_t nextEnemyId = 1; ///< Next enemy ID
    std::unordered_map<uint32_t, std::pair<float, float>> enemies; ///< Maps enemy IDs to positions
    float enemySpawnTimerSec = 0.f; ///< Timer for enemy spawning
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles; ///< Maps obstacle IDs to (x, y, width, height)
    std::unordered_map<uint32_t, std::string> waitingPlayerSkins;
    std::unordered_map<uint32_t, std::string> waitingPlayerWeapons;
    RoomManager roomManager;

public:
	/**
	* @brief Constructs a GameServer instance and binds the UDP socket to the given port.
	* @param port UDP port for client connections.
	*/
	GameServer(uint16_t port);

	/**
	* @brief Main server loop.
	*
	* Waits for clients to connect, assigns IDs, and starts the game when all clients are connected.
	*/
	void run();

	/**
	* @brief Destroys the GameServer.
	*/
	~GameServer() = default;

private:
	/**
	* @brief Handles a ClientHello message from a client.
	* @param data Raw message data.
	* @param clientEndpoint Sender's UDP endpoint.
	*/
	void handleClientHello(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &clientEndpoint);

	/**
	* @brief Handles a ClientRoomIdAsk message from a client.
	* @param data Raw message data.
	* @param from Sender's UDP endpoint.
	*/
	void assignClientToRoom(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);

	/**
	 * @brief Handles a ClientFetchRooms message from a client.
	 * @param data Raw message data.
	 * @param from Sender's UDP endpoint.
	 */
	void handleClientFetchRooms(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);

	/**
	* @brief Broadcasts the GameStart message to all connected clients.
	*/
	void broadcastGameStart();

	void processIncomingPacket(const Connexion::ReceivedPacket &packet);
	void routePacketToGame(const Connexion::ReceivedPacket &packet, MessageType type);

    /**
     * @brief Handles client messages during gameplay (e.g., player input).
     * @param data Raw message data.
     * @param from Sender's UDP endpoint.
     */
    void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

    /**
     * @brief Handles a PlayerSkinUpdate message from a client.
     * @param data Raw message data.
     * @param from Sender's UDP endpoint.
     */
    void handlePlayerSkinUpdate(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

    /**
     * @brief Handles a PlayerWeaponUpdate message from a client.
     * @param data Raw message data.
     * @param from Sender's UDP endpoint.
     */
    void handlePlayerWeaponUpdate(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

	/**
	* @brief Sleeps to maintain a fixed server tick rate.
	* @param start Start time of the current tick.
	* @param tick_ms Desired tick duration in milliseconds.
	*/
	void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point &start, int tick_ms);
};
