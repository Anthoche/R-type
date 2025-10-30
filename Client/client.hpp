/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client v1
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
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <atomic>
#include <utility>
#include <optional>
#include <chrono>
#include <nlohmann/json.hpp>
#include <deque>
#include <map>

class Game;

/**
 * @class GameClient
 * @brief Manages client-side networking for the R-Type multiplayer game.
 *
 * This class handles all communication between the game client and server using both UDP and TCP protocols.
 * It manages game state synchronization, player input transmission, and scene transitions.
 * The client runs a dedicated background thread to receive and process server messages asynchronously.
 */
class GameClient {
    private:
        UDP_socket socket; ///< UDP socket for fast, unreliable communication with the server.
        asio::ip::udp::endpoint serverEndpoint; ///< Network endpoint representing the server's UDP address.
        std::string clientName; ///< Display name of the current player.
        std::thread rxThread; ///< Background thread that continuously receives messages from the server.
        std::atomic<bool> running{false}; ///< Thread-safe flag controlling the client's running state.
        Game &_game; ///< Reference to the main game instance.
        std::unique_ptr<TCP_socket> tcpClient; ///< TCP connection for reliable data transfer (e.g., registry sync).
        std::string serverPortStr; ///< Server port number stored as a string.
        std::string serverIpStr; ///< Server IP address stored as a string.
        bool connectionFailed = false; ///< Flag indicating if the initial connection attempt failed.
        std::string pendingSkinSelection; ///< Stores the skin selection waiting to be sent to the server.
        std::string pendingWeaponSelection; ///< Stores the weapon selection waiting to be sent to the server.
        std::mutex registryMutex; ///< Protects access to the full registry data.
        nlohmann::json latestFullRegistry; ///< Cached copy of the complete game registry received from the server.
        std::atomic<bool> hasPendingFullRegistry{false}; ///< Indicates if a new full registry is ready to be consumed.
        std::atomic<bool> fullRegistryFetchInFlight{false}; ///< Prevents multiple simultaneous registry fetch requests.

        /**
         * @brief Stores a complete registry snapshot received from the server.
         * @param registryJson JSON representation of the full game registry.
         * @param markPending If true, marks the registry as pending for consumption by the game thread.
         */

        void storeFullRegistry(const nlohmann::json &registryJson, bool markPending);

        /**
         * @brief Requests the full game registry from the server asynchronously.
         */
        void fetchFullRegistryAsync();
        
        std::deque<std::pair<std::string, std::string>> _chatQueue; ///< Queue of chat messages waiting to be processed.
        std::mutex roomsMutex; ///< Protects access to the rooms list.
        std::condition_variable roomsCv; ///< Notifies waiting threads when rooms data is updated.
        bool roomsUpdated{false}; ///< Flag indicating if the rooms list has been refreshed.
        std::map<int, game::serializer::RoomData> rooms; ///< Map of available game rooms indexed by room ID.

    public:
        uint32_t clientId{0}; ///< Unique identifier assigned to this client by the server.
        int roomId{-1}; ///< ID of the room the client is currently in (-1 if not in a room).
        std::mutex stateMutex; ///< Protects access to shared game state data structures.

        std::unordered_map<uint32_t, std::tuple<float, float, float>> players; ///< Maps player IDs to their (x, y, z) positions.
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, float, float, float>> obstacles; ///< Maps obstacle IDs to their position, size, and velocity data.
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, uint32_t>> projectiles; ///< Maps projectile IDs to their position, velocity, and owner data.
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, float, float>> enemies; ///< Maps enemy IDs to their position, velocity, and size data.
        std::unordered_map<uint32_t, std::pair<int16_t, int16_t>> playerHealth; ///< Maps player IDs to their (current health, max health).
        std::unordered_map<uint32_t, uint32_t> playerIndividualScores; ///< Maps player IDs to their individual scores.
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, uint32_t>> enemyProjectiles; ///< Maps enemy projectile IDs to their position, velocity, and owner data.
        std::unordered_map<uint32_t, std::string> playerSkins; ///< Maps player IDs to their selected skin filenames.
        std::unordered_map<uint32_t, std::string> playerWeapons; ///< Maps player IDs to their selected weapon identifiers.
        int32_t globalScore = 0; ///< The shared team score for all players.
        std::atomic<bool> bossDefeated{false}; ///< Thread-safe flag indicating if the boss has been defeated.
        bool _lastBoss = false; ///< Flag indicating if the current boss is the final boss of the game.

        /**
         * @brief Maps elements IDs to their (x,y,z,velX,velY,velZ,width,height).
         */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, float, float>> elements;

        /**
         * @brief Waits for the rooms list to be updated by the server.
         * @param timeout Maximum duration to wait for the rooms data.
         * @param outRooms Output parameter that receives a copy of the rooms map if successful.
         * @return true if rooms data was received within the timeout period, false otherwise.
         */
        bool waitForRooms(std::chrono::milliseconds timeout,
                          std::map<int, game::serializer::RoomData> &outRooms);

        /**
         * @brief Constructs a GameClient and establishes a connection to the server.
         * @param game Reference to the main game instance.
         * @param serverIp IP address of the game server.
         * @param serverPort UDP port number of the game server.
         * @param name Display name for this client.
         * @throws std::runtime_error if socket creation or server connection fails.
         */
        GameClient(Game &game, const std::string &serverIp, uint16_t serverPort, const std::string &name);

        /**
         * @brief Destructor that stops the client and cleans up resources.
         */
        ~GameClient();

        /**
         * @brief Starts the client's main networking loop.
         */
        void run();

        /**
         * @brief Sends an initial hello message to the server to establish connection.
         */
        void sendHello();

        /**
         * @brief Requests to join a specific game room.
         * @param roomId The ID of the room to join.
         */
        void sendRoomAsk(uint32_t roomId);

        /**
         * @brief Requests the list of available game rooms from the server.
         */
        void sendRoomsFetch();

        /**
         * @brief Notifies the server that this player is ready to start the game.
         */
        void sendConfirmStart();

        /**
         * @brief Sends a request to leave the current room.
         */
        void sendClientLeaveRoom();

        /**
         * @brief Establishes a TCP connection to the server for reliable data transfer.
         */
        void initTcpConnection();

        /**
         * @brief Main loop that continuously receives and processes UDP messages from the server.
         */
        void recvLoop();

        /**
         * @brief Sends player input events to the server.
         * @param code The type of input (e.g., move up, shoot).
         * @param pressed true if the input is pressed, false if released.
         */
        void sendInputEvent(InputCode code, bool pressed);

        /**
         * @brief Sends the player's current health status to the server.
         * @param health Current health value.
         */
        void sendHealth(int health);

        /**
         * @brief Retrieves and clears the pending full registry if available.
         * @return An optional containing the registry JSON if available, or empty if none pending.
         */
        std::optional<nlohmann::json> consumeFullRegistry();

        /**
         * @brief Notifies the server whether endless mode is enabled.
         * @param isEndless true to enable endless mode, false to disable.
         */
        void sendEndlessMode(bool isEndless);

        /**
         * @brief Sends a chat message to be broadcast to other players.
         * @param message The text content of the chat message.
         */
        void sendChatMessage(const std::string &message);

        /**
         * @brief Gets the display name of this client.
         * @return Constant reference to the client's name.
         */
        const std::string &getClientName() const;

        /**
         * @brief Routes an incoming message to the appropriate handler based on message type.
         * @param type The type of the received message.
         * @param buffer Raw byte data of the message.
         */
        void handleMessage(MessageType type, const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message that assigns a unique ID to this client.
         * @param buffer Raw message data.
         */
        void handleServerAssignId(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message that assigns this client to a room.
         * @param buffer Raw message data.
         */
        void handleServerRoomAssign(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message containing the list of available rooms.
         * @param buffer Raw message data.
         */
        void handleServerRooms(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message indicating the room is ready to start.
         * @param buffer Raw message data.
         */
        void handleRoomReady(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message that signals the game has started.
         * @param buffer Raw message data.
         */
        void handleGameStart(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message containing updated player positions.
         * @param buffer Raw message data.
         */
        void handlePlayerUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Sends the selected player skin to the server.
         * @param skinFilename Filename of the skin asset.
         */
        void sendSkinSelection(const std::string &skinFilename);

        /**
         * @brief Sends the selected weapon to the server.
         * @param weaponId Identifier of the weapon.
         */
        void sendWeaponSelection(const std::string &weaponId);

        /**
         * @brief Processes a server message updating a player's skin.
         * @param buffer Raw message data.
         */
        void handlePlayerSkinUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message updating a player's weapon.
         * @param buffer Raw message data.
         */
        void handlePlayerWeaponUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message spawning a new obstacle.
         * @param buffer Raw message data.
         */
        void handleObstacleSpawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message updating an obstacle's state.
         * @param buffer Raw message data.
         */
        void handleObstacleUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message removing an obstacle.
         * @param buffer Raw message data.
         */
        void handleObstacleDespawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Sends the current scene state to the server.
         * @param scene The current scene (e.g., MENU, GAME).
         * @param registry Pointer to the ECS registry, used when transitioning to the game scene.
         */
        void sendSceneState(SceneState scene, ecs::registry* registry);

        /**
         * @brief Sends a shoot command to the server.
         */
        void sendShoot();

        /**
         * @brief Processes a server message spawning a new projectile.
         * @param buffer Raw message data.
         */
        void handleProjectileSpawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message removing a projectile.
         * @param buffer Raw message data.
         */
        void handleProjectileDespawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message updating a projectile's position.
         * @param buffer Raw message data.
         */
        void handleProjectileUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message removing an enemy.
         * @param buffer Raw message data.
         */
        void handleEnemyDespawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message updating an enemy's state.
         * @param buffer Raw message data.
         */
        void handleEnemyUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message spawning a new enemy.
         * @param buffer Raw message data.
         */
        void handleEnemySpawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message indicating a boss has been defeated.
         * @param buffer Raw message data.
         */
        void handleBossDeath(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message indicating a player has died.
         * @param buffer Raw message data.
         */
        void handlePlayerDeath(const std::vector<uint8_t> &buffer);

        /**
         * @brief Checks if the connection to the server has failed.
         * @return true if connection failed, false otherwise.
         */
        bool hasConnectionFailed() const;

        /**
         * @brief Checks if the client is successfully connected to the server.
         * @return true if connected (has received a client ID), false otherwise.
         */
        bool isConnected() const;

        /**
         * @brief Processes a server message updating a player's health.
         * @param buffer Raw message data.
         */
        void handlePlayerHealth(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message updating the global team score.
         * @param buffer Raw message data.
         */
        void handleGlobalScore(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message updating a player's individual score.
         * @param buffer Raw message data.
         */
        void handleIndividualScore(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message spawning a new enemy projectile.
         * @param buffer Raw message data.
         */
        void handleEnemyProjectileSpawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message updating an enemy projectile's position.
         * @param buffer Raw message data.
         */
        void handleEnemyProjectileUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message removing an enemy projectile.
         * @param buffer Raw message data.
         */
        void handleEnemyProjectileDespawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message setting the initial health values.
         * @param buffer Raw message data.
         */
        void handleInitialHealth(const std::vector<uint8_t> &buffer);

        /**
         * @brief Processes a server message regarding endless mode settings.
         * @param buffer Raw message data.
         */
        void handleEndlessMode(const std::vector<uint8_t> &buffer);

         /**
         * @brief Handles an element despawn message.
         * @param buffer Raw message data.
         */
        void handleElementDespawn(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an element update message.
         * @param buffer Raw message data.
         */
        void handleElementUpdate(const std::vector<uint8_t> &buffer);

        /**
         * @brief Handles an Element spawn message.
         * @param buffer Raw message data.
         */
        void handleElementSpawn(const std::vector<uint8_t> &buffer);
        /**
         * @brief Processes a server message broadcasting a chat message from another player.
         * @param buffer Raw message data.
         */
        void handleChatMessage(const std::vector<uint8_t> &buffer);

        /**
         * @brief Retrieves and clears all pending chat messages received from the network.
         * @return Vector of (sender name, message content) pairs.
         */
        std::vector<std::pair<std::string, std::string>> consumeChatMessages();
};