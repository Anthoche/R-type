/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ServerGame - server-authoritative game loop
*/

#pragma once

#include <unordered_map>
#include <tuple>
#include <utility>
#include <cstdint>
#include <chrono>
#include "../Shared/protocol.hpp"
#include "../Server/Network_handler/Include/UDP_socket.hpp"

/**
 * @class ServerGame
 * @brief Server-authoritative game loop for R-Type.
 *
 * This class manages the state of the game on the server side:
 * - Tracks players, enemies, and obstacles.
 * - Processes client messages.
 * - Spawns and updates enemies.
 * - Broadcasts state updates to all connected clients.
 */
class ServerGame {
    UDP_socket &socket; ///< Reference to the UDP socket used for communication.

    // --- Game state ---
    std::unordered_map<uint32_t, std::pair<float,float>> playerPositions; ///< Player positions keyed by player ID.
    uint32_t nextEnemyId = 1; ///< Counter used to assign unique IDs to new enemies.
    std::unordered_map<uint32_t, std::pair<float,float>> enemies; ///< Enemy positions keyed by enemy ID.
    float enemySpawnTimerSec = 0.f; ///< Timer controlling enemy spawning.

    std::unordered_map<uint32_t, std::tuple<float,float,float,float>> obstacles; ///< Obstacles keyed by ID (x,y,w,h).

public:
    /**
     * @brief Construct a new ServerGame.
     * @param sock Reference to the UDP socket used for client communication.
     */
    explicit ServerGame(UDP_socket &sock);

    /**
     * @brief Run the main server game loop.
     *
     * This loop processes client messages, updates game state,
     * spawns/despawns enemies and obstacles, and broadcasts updates.
     */
    void run();

private:
    /**
     * @brief Initialize player positions at the start of the game.
     */
    void initialize_player_positions();

    /**
     * @brief Initialize obstacles at the start of the game.
     */
    void initialize_obstacles();

    /**
     * @brief Process any pending network messages from clients.
     */
    void process_pending_messages();

    /**
     * @brief Handle a single message from a client.
     * @param data Raw message data received from the client.
     * @param from Client network address.
     */
    void handle_client_message(const std::vector<uint8_t>& data, const sockaddr_in& from);

    /**
     * @brief Update enemy states.
     * @param dt Delta time (in seconds) since the last update.
     */
    void update_enemies(float dt);

    /**
     * @brief Spawn a new enemy into the game world.
     */
    void spawn_enemy();

    /**
     * @brief Broadcast the current state of all entities to connected clients.
     */
    void broadcast_states_to_clients();

    /**
     * @brief Notify clients that a new enemy has spawned.
     * @param enemyId Unique ID of the enemy.
     * @param x X-coordinate of the enemy.
     * @param y Y-coordinate of the enemy.
     */
    void broadcast_enemy_spawn(uint32_t enemyId, float x, float y);

    /**
     * @brief Notify clients of an enemy position update.
     * @param enemyId Unique ID of the enemy.
     * @param x Updated X-coordinate.
     * @param y Updated Y-coordinate.
     */
    void broadcast_enemy_update(uint32_t enemyId, float x, float y);

    /**
     * @brief Notify clients that an enemy has been removed.
     * @param enemyId ID of the despawned enemy.
     */
    void broadcast_enemy_despawn(uint32_t enemyId);

    /**
     * @brief Notify clients that an obstacle has spawned.
     * @param obstacleId Unique ID of the obstacle.
     * @param x X-coordinate of the obstacle.
     * @param y Y-coordinate of the obstacle.
     * @param w Width of the obstacle.
     * @param h Height of the obstacle.
     */
    void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);

    /**
     * @brief Notify clients that an obstacle has been removed.
     * @param obstacleId ID of the despawned obstacle.
     */
    void broadcast_obstacle_despawn(uint32_t obstacleId);

    /**
     * @brief Sleep to maintain a fixed tick rate for the server loop.
     * @param start Start time of the current tick.
     * @param tick_ms Duration of one tick in milliseconds.
     */
    void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};
