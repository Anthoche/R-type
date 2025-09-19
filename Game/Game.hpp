/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Game
*/

#ifndef RTYPE_GAME_HPP
#define RTYPE_GAME_HPP

#include <string>
#include <thread>
#include "../Client/Network_client/Include/client.hpp"
#include "../Engine/Rendering/scene/SceneHandler.hpp"

/**
 * @class Game
 * @brief Main client-side game class.
 * 
 * Handles the game loop, networking, and scene management for the R-Type client.
 */
class Game {
public:
    /**
     * @brief Constructs a Game instance.
     * 
     * Initializes the client and scene handler, prepares the networking thread.
     * 
     * @param serverIp IP address of the game server.
     * @param clientName Name of the player/client.
     */
    Game(const std::string &serverIp, const std::string &clientName);

    /**
     * @brief Destructor for Game.
     */
    ~Game() = default;

    /**
     * @brief Runs the main game loop.
     * 
     * Starts the rendering, input handling, and client updates.
     */
    void run();

private:
    GameClient _client;             ///< Network client handling server communication
    std::thread _networkThread;     ///< Thread for handling network communication
    SceneHandler _sceneHandler;     ///< Scene manager for rendering and switching scenes
};

#endif // RTYPE_GAME_HPP
