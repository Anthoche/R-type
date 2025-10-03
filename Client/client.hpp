/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#pragma once

#include "../Shared/protocol.hpp"
#include "../Engine/Utils/Include/serializer.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
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
 * @brief UDP client for the R-Type multiplayer game.
 *
 * Manages network communication with the server, handles game state updates,
 * and runs two main loops:
 * - A network thread for receiving messages.
 * - A render thread for user input and display.
 */
class GameClient {
private:
    int socketFd; ///< File descriptor for the UDP socket.
    sockaddr_in serverAddr{}; ///< Server address structure.
    uint32_t clientId{0}; ///< Unique client ID assigned by the server.
    std::string clientName; ///< Name of this client.
    std::thread rxThread; ///< Thread for receiving network messages.
    std::atomic<bool> running{false}; ///< Flag to control the client lifecycle.
    Game &_game; ///< Reference to the game instance.

    /**
     * @brief Scene state (menu, game, etc.).
     */
    std::atomic<SceneState> currentScene{SceneState::MENU};

public:
    /**
     * @brief Temporarily exposed for state access.
     */
    std::mutex stateMutex; ///< Mutex to protect shared game state.

    /**
     * @brief Maps client IDs to their (x,y) positions.
     */
    std::unordered_map<uint32_t, std::pair<float, float>> players;

    /**
     * @brief Maps obstacle IDs to their (x,y,width,height).
     */
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles;

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
     * @brief Destroys the GameClient, cleans up resources, and stops all threads.
     */
    ~GameClient();

    /**
     * @brief Starts the client main loop.
     *
     * Launches the network receive thread and waits for the game to start.
     */
    void run();

    /**
     * @brief Runs the main render and input loop.
     *
     * Handles user input, sends movement updates to the server,
     * and renders the game state (players, obstacles).
     */
    void runRenderLoop();

    /**
     * @brief Sends a ClientHello message to the server to initiate connection.
     */
    void sendHello();

    /**
     * @brief Sends player input to the server.
     * @param inputX Horizontal input (-1 to 1).
     * @param inputY Vertical input (-1 to 1).
     */
    void sendInput(float inputX, float inputY);

    /**
     * @brief Sends the current scene state to the server.
     */
    void sendSceneState();

    /**
     * @brief Network receive loop.
     *
     * Listens for incoming messages and dispatches them to the appropriate handler.
     */
    void recvLoop();

    /**
     * @brief Dispatches an incoming message to the appropriate handler.
     * @param type Type of the message.
     * @param buffer Raw message data.
     */
    void handleMessage(MessageType type, const std::vector<uint8_t> &buffer);

    // --- Player-related message handlers ---
    void handleServerAssignId(const std::vector<uint8_t> &buffer);
    void handleGameStart(const std::vector<uint8_t> &buffer);
    void handlePlayerUpdate(const std::vector<uint8_t> &buffer);

    // --- Obstacle-related message handlers ---
    void handleObstacleSpawn(const std::vector<uint8_t> &buffer);
    void handleObstacleDespawn(const std::vector<uint8_t> &buffer);

    ecs::entity_t handleEntityCreation(const nlohmann::json& j);

    // --- Scene state accessors ---
    void setSceneState(const std::string &sceneName) {
        if (sceneName == "menu")
            currentScene = SceneState::MENU;
        else if (sceneName == "game")
            currentScene = SceneState::GAME;
        else
            currentScene = SceneState::UNKNOWN;
    }

    SceneState getSceneState() const {
        return currentScene.load();
    }
};

