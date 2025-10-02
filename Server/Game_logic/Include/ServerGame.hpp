/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/
#pragma once

#include <unordered_map>
#include <tuple>
#include <utility>
#include <cstdint>
#include <chrono>
#include <vector>
#include <sys/socket.h>
#include <nlohmann/json.hpp>

#include "../../../Shared/protocol.hpp"
#include "../../Network_handler/Include/UDP_socket.hpp"
#include "../../../Engine/Core/Include/registry.hpp"
#include "../../../Engine/Utils/Include/entity_storage.hpp"

class ServerGame {
    UDP_socket &socket; ///< Reference to the UDP socket used for communication.
    std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Player positions keyed by player ID.
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles; ///< Obstacles keyed by ID (x,y,w,h).
    ecs::registry registry_server; ///< ECS registry for managing entities and components.
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> projectiles; // id -> (x,y,vx,vy)
    uint32_t nextProjectileId = 1;


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
         * and broadcasts updates.
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
         * @brief Broadcast the current state of all entities to connected clients.
         */
        void broadcast_states_to_clients();

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

        /**
         * @brief Updates all active projectiles on the server side.
         * @param dt Delta time in seconds since the last update.
         */
        void update_projectiles_server_only(float dt);

        /**
         * @brief Broadcasts the current positions of all active projectiles to all clients.
         */
        void broadcast_projectile_positions();

        /**
         * @brief Checks for collisions between projectiles and other game entities.
         */
        void check_projectile_collisions();

        /**
         * @brief Notifies all clients of a newly spawned projectile.
         * @param projId Unique ID of the projectile.
         * @param ownerId ID of the entity (player or enemy) that fired it.
         * @param x Initial X coordinate.
         * @param y Initial Y coordinate.
         * @param vx Horizontal velocity.
         * @param vy Vertical velocity.
         */
        void broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId, float x, float y, float vx, float vy);

        /**
         * @brief Notifies all clients that a projectile has been removed from the game.
         * @param projId Unique ID of the projectile to despawn.
         */
        void broadcast_projectile_despawn(uint32_t projId);
};
