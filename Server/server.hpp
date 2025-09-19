/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#pragma once

#include "Network_handler/Include/UDP_socket.hpp"
#include "../Shared/protocol.hpp"
#include <thread>
#include <atomic>
#include <unordered_map>

/**
 * @brief Main server class for the R-Type multiplayer game.
 *
 * Handles client connections, game state, and network messages.
 * Manages up to 4 players, enemy spawning, and obstacle placement.
 */
class GameServer {
    UDP_socket socket; ///< Network socket for UDP communication.
    std::atomic<bool> gameStarted{false}; ///< Flag indicating if the game has started.
    uint32_t nextClientId = 1; ///< Next available client ID.
    std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Maps client IDs to their (x,y) positions.
    uint32_t nextEnemyId = 1; ///< Next available enemy ID.
    std::unordered_map<uint32_t, std::pair<float, float>> enemies; ///< Maps enemy IDs to their (x,y) positions.
    float enemySpawnTimerSec = 0.f; ///< Timer for enemy spawning logic.
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles; ///< Maps obstacle IDs to their (x,y,width,height).

    public:
        /**
         * @brief Constructs a GameServer instance.
         * @param port The UDP port to listen on.
         */
        GameServer(uint16_t port);

        /**
         * @brief Main server loop.
         *
         * Waits for 4 clients to connect, then starts the game.
         * Delegates game logic to ServerGame after all clients are connected.
         */
        void run();

        ~GameServer() = default;

    private:
        /**
         * @brief Handles a new client connection request.
         * @param data Received network data (expected: ClientHelloMessage).
         * @param clientAddr Address of the connecting client.
         *
         * Assigns a unique ID to the client and sends it back.
         * Starts the game if 4 clients are connected.
         */
        void handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr);

        /**
         * @brief Broadcasts the game start message to all connected clients.
         *
         * Sent when 4 clients are ready.
         */
        void broadcastGameStart();

        /**
         * @brief Processes a message from a connected client.
         * @param data Received network data (expected: ClientInputMessage).
         * @param from Address of the client who sent the message.
         *
         * Updates player position based on input and clamps to window bounds.
         */
        void handle_client_message(const std::vector<uint8_t>& data, const sockaddr_in& from);

        /**
         * @brief Sleeps to maintain a fixed game tick rate.
         * @param start Timestamp at the beginning of the tick.
         * @param tick_ms Desired tick duration in milliseconds.
         */
        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};
