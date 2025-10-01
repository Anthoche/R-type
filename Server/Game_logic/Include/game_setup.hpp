/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** game_setup
*/

#pragma once

#include "../../Network_handler/Include/connexion.hpp"
#include "../../../Shared/protocol.hpp"
#include <atomic>

/**
 * @brief Manages the initial player connection phase and game startup.
 *
 * Handles client hello messages, assigns client IDs, and triggers the game start
 * once 4 players are connected. Uses Connexion for network operations.
 */
class GameSetup {
    public:
        /**
         * @brief Constructs a GameSetup instance.
         * @param connexion Reference to the network connection manager.
         */
        GameSetup(Connexion& connexion);

        ~GameSetup() = default;

        /**
         * @brief Waits for 4 players to connect.
         * @return true once all players are connected and the game is ready to start.
         *
         * Listens for ClientHello messages and assigns unique IDs to each client.
         * Sets gameStarted to true when 4 clients are connected.
         */
        bool waitForPlayers();

        /**
         * @brief Broadcasts the GameStart message to all connected clients.
         *
         * Called automatically once 4 players are connected.
         */
        void broadcastGameStart();

    private:
        Connexion& connexion; ///< Reference to the network connection manager.
        std::atomic<uint32_t> nextClientId{1}; ///< Next available client ID.
        std::atomic<bool> gameStarted{false}; ///< Flag indicating if the game has started.

        /**
         * @brief Processes a ClientHello message from a new player.
         * @param data Received network data (expected: ClientHelloMessage).
         * @param clientAddr Address of the connecting client.
         *
         * Assigns a unique ID to the client and sends a ServerAssignId response.
         * Sets gameStarted to true if 4 clients are connected.
         */
        void handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr);
};
