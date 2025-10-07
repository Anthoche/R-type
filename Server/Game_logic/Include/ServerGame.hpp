/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/
#pragma once
#include "../../../Shared/protocol.hpp"
#include "../../../Engine/Core/Include/registry.hpp"
#include "../../../Engine/Utils/Include/entity_storage.hpp"
#include "../../Network_handler/Include/connexion.hpp"
#include <asio/ip/udp.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <mutex>

/**
 * @class ServerGame
 * @brief Main server-side game logic handler.
 *
 * Manages the game loop, player states, ECS registry, and network broadcasting.
 */
class ServerGame {
    public:
        /**
         * @brief Constructs a ServerGame instance.
         * @param conn Reference to the Connexion handler for network communication.
         */
        ServerGame(Connexion &conn);

        /**
         * @brief Runs the main server game loop.
         *
         * Handles message processing, state updates, and broadcasting.
         */
        void run();

        /**
         * @brief Loads players from a JSON configuration file.
         * @param path Path to the players JSON file.
         */
        void load_players(const std::string &path);

        /**
         * @brief Loads level entities from a JSON configuration file.
         * @param path Path to the level JSON file.
         */
        void load_level(const std::string &path);

        /**
         * @brief Broadcasts the entire ECS registry to all clients.
         *
         * Serializes all entities and sends them in batches.
         */
        void broadcast_full_registry_to(uint32_t clientId);

    private:
        Connexion &connexion;
        ecs::registry registry_server;
        std::unordered_map<uint32_t, SceneState> clientScenes;
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles;
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> projectiles;
        uint32_t nextProjectileId = 1;
        std::mutex mtx;
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> enemies;
        std::unordered_set<uint32_t> deadPlayers;
        uint32_t nextEnemyId = 1;

        /**
         * @brief Initializes player positions based on connected clients.
         */
        void initialize_player_positions();
        
        /**
         * @brief Processes pending network messages from clients.
         */
        void process_pending_messages();

        /**
         * @brief Handles a single client message.
         * @param data Raw message data.
         * @param from Client endpoint.
         */
        void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);  // <-- Changement ici
        
        /**
         * @brief Broadcasts player state updates to all clients.
         */
        void broadcast_states_to_clients();

        /**
         * @brief Checks for collisions between players and enemies.
         */
        void check_player_enemy_collisions();

        /**
         * @brief Notifies all clients that a player has died.
         * @param clientId ID of the player who died.
         */
        void broadcast_player_death(uint32_t clientId);

        /**
         * @brief Initializes obstacles in the game world.
         */
        void initialize_obstacles();
        
        /**
         * @brief Broadcasts an obstacle spawn message.
         * @param obstacleId Unique obstacle ID.
         * @param x X position.
         * @param y Y position.
         * @param w Width.
         * @param h Height.
         */
        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);

        /**
         * @brief Broadcasts an obstacle despawn message.
         * @param obstacleId Unique obstacle ID.
         */
        void broadcast_obstacle_despawn(uint32_t obstacleId);

        /**
         * @brief Sleeps to maintain consistent tick rate.
         * @param start Tick start time.
         * @param tick_ms Target tick duration in milliseconds.
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


        /**
         * @brief Initializes enemies in the game world.
         */
        void initialize_enemies(); 

        /**
         * @brief Broadcasts an enemy spawn message.
         * @param enemyId Unique enemy ID.
         * @param x X position.
         * @param y Y position.
         * @param vx Velocity in X direction.
         * @param vy Velocity in Y direction.
         */
        void broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float vx, float vy); 

        /**
         * @brief Broadcasts the current positions of all active enemies to all clients.
         */
        void broadcast_enemy_positions();

        /**
         * @brief Updates all active enemies on the server side.
         * @param dt Delta time in seconds since the last update.
         */
        void update_enemies(float dt);

        /**
         * @brief Checks for collisions between projectiles and enemies.
         */
        void check_projectile_enemy_collisions();

        /**
         * @brief Notifies all clients that an enemy has been removed from the game.
         * @param enemyId Unique ID of the enemy to despawn.
         */
        void broadcast_enemy_despawn(uint32_t enemyId);
        
        /**
         * @brief Notifies all clients of an enemy's updated position.
         * @param enemyId Unique ID of the enemy.
         * @param x New X coordinate.
         * @param y New Y coordinate.
         */
        void broadcast_enemy_update(uint32_t enemyId, float x, float y);
};
