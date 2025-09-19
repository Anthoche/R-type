/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** game_core
*/

#pragma once

#include "connexion.hpp"
#include "protocol.hpp"
#include <unordered_map>
#include <chrono>

/**
 * @brief Core game logic manager.
 *
 * Handles the main game loop, player input, enemy spawning/updating,
 * and state synchronization between server and clients at ~60Hz.
 */
class GameCore {
    public:
        /**
         * @brief Constructs a GameCore instance.
         * @param connexion Reference to the network connection manager.
         */
        GameCore(Connexion& connexion);

        ~GameCore() = default;

        /**
         * @brief Runs the main game loop.
         *
         * Initializes player positions, then processes input, updates game state,
         * and broadcasts updates to clients at a fixed tick rate (~60Hz).
         */
        void run();

    private:
        Connexion& connexion; ///< Reference to the network connection manager.
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Maps client IDs to their (x,y) positions.
        std::unordered_map<uint32_t, std::pair<float, float>> enemies; ///< Maps enemy IDs to their (x,y) positions.
        uint32_t nextEnemyId = 1; ///< Next available enemy ID.
        float enemySpawnTimerSec = 0.f; ///< Timer for enemy spawning logic.

        /**
         * @brief Initializes player starting positions.
         */
        void initializePlayerPositions();

        /**
         * @brief Processes all pending client messages.
         *
         * Handles ClientInput messages to update player positions.
         */
        void processPendingMessages();

        /**
         * @brief Broadcasts current player positions to all clients.
         */
        void broadcastStatesToClients();

        /**
         * @brief Updates enemy positions and spawns new enemies as needed.
         * @param dt Delta time since last update (in seconds).
         */
        void updateEnemies(float dt);

        /**
         * @brief Spawns a new enemy at a random position.
         */
        void spawnEnemy();

        /**
         * @brief Broadcasts an enemy spawn event to all clients.
         * @param enemyId ID of the spawned enemy.
         * @param x Initial x-coordinate.
         * @param y Initial y-coordinate.
         */
        void broadcastEnemySpawn(uint32_t enemyId, float x, float y);

        /**
         * @brief Broadcasts an enemy position update to all clients.
         * @param enemyId ID of the enemy to update.
         * @param x Current x-coordinate.
         * @param y Current y-coordinate.
         */
        void broadcastEnemyUpdate(uint32_t enemyId, float x, float y);

        /**
         * @brief Broadcasts an enemy despawn event to all clients.
         * @param enemyId ID of the enemy to despawn.
         */
        void broadcastEnemyDespawn(uint32_t enemyId);

        /**
         * @brief Handles a message from a connected client.
         * @param data Received network data (expected: ClientInputMessage).
         */
        void handleClientMessage(const std::vector<uint8_t>& data);

        /**
         * @brief Sleeps to maintain a fixed game tick rate.
         * @param start Timestamp at the beginning of the tick.
         * @param tick_ms Desired tick duration in milliseconds.
         */
        void sleepToMaintainTick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};
