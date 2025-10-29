/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** IServerGame
*/

#pragma once

#include "connexion.hpp"
#include "../../Shared/protocol.hpp"
#include "../../Engine/Utils/Include/serializer.hpp"
#include "../../Engine/Core/Include/registry.hpp"
#include "../../Engine/Utils/Include/entity_storage.hpp"
#include <cstdint>
#include <vector>

/**
 * @class IServerGame
 * @brief Interface for server-side game instance management.
 *
 * This interface defines the contract for game instances running on the server.
 * Each game instance manages gameplay logic, processes player input packets,
 * synchronizes game state, and handles the ECS registry for a specific game room.
 * Implementations of this interface run the core game loop and physics simulation.
 */
class IServerGame {
    public:
        /**
         * @brief Virtual destructor for proper cleanup of derived classes.
         */
        virtual ~IServerGame() = default;

        /**
         * @brief Starts the game instance's main loop for a specific room.
         *
         * Initializes the game state, begins the gameplay loop, and handles
         * all game logic until the game ends or is interrupted.
         *
         * @param roomId The unique identifier of the room this game instance manages.
         */
        virtual void run(int roomId) = 0;

        /**
         * @brief Enqueues a network packet for processing by the game instance.
         *
         * Receives player input and other game-related packets from the network layer
         * and queues them for processing in the game loop.
         *
         * @param data Raw packet data received from a client.
         * @param from The UDP endpoint (address and port) of the packet sender.
         */
        virtual void enqueuePacket(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) = 0;

        /**
         * @brief Sets the initial list of clients participating in this game.
         *
         * Called before the game starts to establish which players are in the room.
         * The boolean value indicates whether each client is ready to start.
         *
         * @param clients Map of client IDs to their ready status (true = ready, false = not ready).
         */
        virtual void setInitialClients(const std::map<uint32_t, bool> &clients) = 0;
};