/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** game_setup
*/

#pragma once

#include "connexion.hpp"
#include "../../../Shared/protocol.hpp"
#include <atomic>
#include <iostream>

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
         * @brief Starts waiting asynchronously for players to connect.
         * 
         * Once 4 players are connected, gameStarted is set to true.
         */
        void startWaiting();

        /**
         * @brief Broadcasts the GameStart message to all connected clients.
         */
        void broadcastGameStart();
        
        /**
         * @brief Checks if the game has started.
         * @return true if the game has started, false otherwise.
         */
        bool isGameStarted() const;
    private:
        Connexion& connexion; ///< Reference to the network connection manager.
        std::atomic<uint32_t> nextClientId{1}; ///< Next available client ID.
        std::atomic<bool> gameStarted{false}; ///< Flag indicating if the game has started.

        /**
         * @brief Asynchronously wait for the next client message.
         */
        void doAsyncReceive();

        /**
         * @brief Processes a ClientHello message from a new player.
         */
        void handleClientHello(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& clientAddr);
};