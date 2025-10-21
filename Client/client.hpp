/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#pragma once

#include "../Shared/protocol.hpp"
#include "../Shared/Sockets/Include/UDP_socket.hpp"
#include "../Shared/Sockets/Include/TCP_socket.hpp"
#include "../Engine/Core/Include/registry.hpp"
#include "serializer.hpp"
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
        UDP_socket socket; ///< UDP socket used for communication with the server (client mode).
        asio::ip::udp::endpoint serverEndpoint; ///< Server endpoint for UDP messages.
        std::string clientName; ///< Name of the current client.
        std::thread rxThread; ///< Thread used for receiving messages.
        std::atomic<bool> running{false}; ///< Flag to control client activity.
        Game &_game; ///< Reference to the associated Game instance.
        std::unique_ptr<TCP_socket> tcpClient; ///< TCP client for reliable data transfer (client mode).
        std::string serverPortStr; ///< Server port address as a string.
        std::string serverIpStr; ///< Server IP address as a string.
        bool connectionFailed = false;

    public:
        uint32_t clientId{0}; ///< Unique client ID assigned by the server.
        int roomId{-1};
        std::map<int, game::serializer::RoomData> rooms;
        
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
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, uint32_t>> projectiles;

        /**
         * @brief Maps enemy IDs to their (x,y,velX,velY).
         */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> enemies;

        /**
         * @brief Maps player IDs to their current health values.
         */
        std::unordered_map<uint32_t, std::pair<int16_t, int16_t>> playerHealth;

        /**
         * @brief Maps player IDs to their individual scores.
         */
        std::unordered_map<uint32_t, uint32_t> playerIndividualScores;

        /**
         * @brief Maps enemy projectile IDs to their (x,y,velX,velY,ownerId).
         */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, uint32_t>> enemyProjectiles;

        int32_t globalScore = 0;

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
         * @brief Sends a "hello" message to the server to initiate connection.
         */
        void sendHello();

        /**
         * Sends a room connect request to the server
         * @param roomId The id of the room to join
         */
        void sendRoomAsk(uint32_t roomId);

        /**
         * @brief Sends a ClientFetchRooms request to the server in order to retrieve available rooms
         */
        void sendRoomsFetch();

        /**
         * @brief Initializes a TCP connection to the server.
         *
         * Uses the client's assigned ID to compute a unique TCP port.
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
         * @brief Handles a ServerRoomAssignId message from the server
         * @param buffer Raw Message data
         */
        void handleServerRoomAssign(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles a ServerSendRooms message from the server
         * @param buffer Raw message data
         */
        void handleServerRooms(const std::vector<uint8_t> &buffer);

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

        /**
         * @brief Vérifie si la connexion au serveur a échoué
         * @return true si échec, false sinon
         */
        bool hasConnectionFailed() const;

        /**
         * @brief Vérifie si le client est connecté au serveur
         * @return true si connecté (a reçu un clientId), false sinon
         */
        bool isConnected() const;
  
        /**
         * @brief Handles a player health update message.
         * @param buffer Raw message data.
         */
        void handlePlayerHealth(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles a global score update message.
         * @param buffer Raw message data.
         */
        void handleGlobalScore(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an individual score update message.
         * @param buffer Raw message data.
         */
        void handleIndividualScore(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an enemy projectile spawn message.
         * @param buffer Raw message data.
         */
        void handleEnemyProjectileSpawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an enemy projectile update message.
         * @param buffer Raw message data.
         */
        void handleEnemyProjectileUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an enemy projectile despawn message.
         * @param buffer Raw message data.
         */
        void handleEnemyProjectileDespawn(const std::vector<uint8_t> &buffer);
};