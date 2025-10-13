/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#pragma once

#include "../Shared/protocol.hpp"
#include "Network_client/Include/UDP_socket.hpp"
#include "Network_client/Include/TCP_socketClient.hpp"
#include "../../Core/Include/registry.hpp"
#include <asio.hpp>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <utility>

class Game;

/**
 * @class GameClient
 * @brief UDP and TCP client for the R-Type multiplayer game.
 *
 * This class manages communication with the game server using both UDP and TCP.
 * It handles:
 * - Game state synchronization.
 * - Player input transmission.
 * - Scene changes and registry updates.
 *
 * The client runs a dedicated thread for receiving server messages asynchronously.
 */
class GameClient {
    private:
        UDP_socket socket; ///< UDP socket used for communication with the server.
        asio::ip::udp::endpoint serverEndpoint; ///< Server endpoint for UDP messages.
        std::string clientName; ///< Name of the current client.
        std::thread rxThread; ///< Thread used for receiving messages.
        std::atomic<bool> running{false}; ///< Flag to control client activity.
        Game &_game; ///< Reference to the associated Game instance.
        std::unique_ptr<TCP_socketClient> tcpClient; ///< TCP client for reliable data transfer.
        std::string serverIpStr; ///< Server IP address as a string.
    public:
        uint32_t clientId{0}; ///< Unique client ID assigned by the server.
        
        /**
         * @brief Mutex used to protect shared game state access.
         */
        std::mutex stateMutex;

        /**
         * @brief Maps client IDs to their (x, y) positions in the game world.
         */
        std::unordered_map<uint32_t, std::pair<float, float>> players;

        /**
         * @brief Maps obstacle IDs to their (x, y, width, height) values.
         */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles;

        /**
         * @brief Maps projectiles IDs to their (x,y,width,height).
         */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> projectiles;

        /**
         * @brief Maps enemy IDs to their (x,y,velX,velY).
         */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> enemies;

        /**
         * @brief Constructs a GameClient and connects to the server.
         * @param game The game instance.
         * @param serverIp IP address of the game server.
         * @param serverPort UDP port of the game server.
         * @param name Name of this client.
         * @throws std::runtime_error if socket creation or server address resolution fails.
         */
        GameClient(Game &game, const std::string &serverIp, uint16_t serverPort, const std::string &name);

        /**
         * @brief Destroys the GameClient and stops background threads.
         */
        ~GameClient();

        /**
         * @brief Starts the client logic.
         *
         * Launches the network receive loop and waits for the game to start.
         */
        void run();

        /**
         * @brief Main rendering and input loop (executed after game start).
         *
         * Handles user input, sends commands to the server, and updates visuals.
         */
        void runRenderLoop();

        /**
         * @brief Sends a "hello" message to the server to initiate connection.
         */
        void sendHello();

        /**
         * @brief Initializes a TCP connection to the server.
         *
         * Uses the client’s assigned ID to compute a unique TCP port.
         */
        void initTcpConnection();

        /**
         * @brief Main UDP receive loop.
         *
         * Continuously listens for incoming UDP packets and dispatches them
         * to the appropriate handler based on message type.
         */
        void recvLoop();

        /**
         * @brief Sends player movement input to the server.
         *
         * @param inputX Horizontal input (-1.0 to 1.0).
         * @param inputY Vertical input (-1.0 to 1.0).
         */
        void sendInput(float inputX, float inputY);

        /**
         * @brief Handles an incoming message from the server.
         *
         * @param type The type of the message.
         * @param buffer The raw message buffer.
         */
        void handleMessage(MessageType type, const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles a ServerAssignId message from the server.
         * @param buffer Raw message data.
         */
        void handleServerAssignId(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles a GameStart message, signaling the start of gameplay.
         * @param buffer Raw message data.
         */
        void handleGameStart(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles a StateUpdate message (updates player positions).
         * @param buffer Raw message data.
         */
        void handlePlayerUpdate(const std::vector<uint8_t> &buffer);


        /**
         * @brief Handles an ObstacleSpawn message.
         * @param buffer Raw message data.
         */
        void handleObstacleSpawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an ObstacleDespawn message.
         * @param buffer Raw message data.
         */
        void handleObstacleDespawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Sends the current scene state to the server.
         *
         * Used to inform the server whether the client is in MENU or GAME state.
         *
         * @param scene The current SceneState (MENU, GAME, UNKNOWN).
         * @param registry Pointer to the current ECS registry (used when switching to GAME).
         */
        void sendSceneState(SceneState scene, ecs::registry* registry);

        /**
         * @brief Handles an shoot message.
         */
        void sendShoot();

        /**
         * @brief Handles an projectiles spwan message.
         * @param buffer Raw message data.
         */
        void handleProjectileSpawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an projectiles despwan message.
         * @param buffer Raw message data.
         */
        void handleProjectileDespawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an projectiles update message.
         * @param buffer Raw message data.
         */
        void handleProjectileUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an enemy despwan message.
         * @param buffer Raw message data.
         */
        void handleEnemyDespawn(const std::vector<uint8_t> &buffer);
        /**
         * @brief Handles an enemy update message.
         * @param buffer Raw message data.
         */
        void handleEnemyUpdate(const std::vector<uint8_t> &buffer);
        /**
         * @brief Handles an enemy spwan message.
         * @param buffer Raw message data.
         */
        void handleEnemySpawn(const std::vector<uint8_t> &buffer);
        /**
         * @brief Handles a player death message.
         * @param buffer Raw message data.
         */
        void handlePlayerDeath(const std::vector<uint8_t> &buffer);
};
