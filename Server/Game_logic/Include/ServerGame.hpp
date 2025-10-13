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
 * @brief Manages the server-side game logic for R-Type multiplayer gameplay.
 *
 * This class is responsible for:
 * - Running the main game loop at a fixed tick rate (16ms).
 * - Managing player positions, health, and scores.
 * - Handling collisions between projectiles, enemies, obstacles, and players.
 * - Spawning and updating enemies with various behavior patterns.
 * - Broadcasting game state updates to all connected clients.
 * - Processing client input messages (movement, shooting, scene changes).
 */
class ServerGame {
    public:
        /**
         * @brief Constructs a ServerGame instance.
         * @param conn Reference to the Connexion handler for network communication.
         */
        ServerGame(Connexion &conn);

        /**
         * @brief Main server game loop.
         *
         * Runs indefinitely at ~60 FPS (16ms tick rate). Each iteration:
         * - Updates projectiles and enemies.
         * - Checks all collision types (projectile-obstacle, projectile-enemy, player-enemy).
         * - Broadcasts updated positions and game state to clients.
         * - Processes incoming client messages.
         * - Maintains consistent tick timing.
         */
        void run();

        /**
         * @brief Loads player data from a JSON configuration file.
         * @param path Path to the JSON file containing player data.
         * @throws std::runtime_error if the file cannot be opened or parsed.
         */
        void load_players(const std::string &path);

        /**
         * @brief Loads level entities from a JSON configuration file.
         * @param path Path to the JSON file containing level data.
         * @throws std::runtime_error if the file cannot be opened or parsed.
         */
        void load_level(const std::string &path);

        /**
         * @brief Sends the complete game registry state to a specific client.
         * @param clientId The ID of the client to send the registry to.
         */
        void broadcast_full_registry_to(uint32_t clientId);

    private:
        /** @brief Reference to the network connection handler. */
        Connexion &connexion;

        /** @brief ECS registry containing all server-side game entities and components. */
        ecs::registry registry_server;

        /** @brief Maps client IDs to their current scene state (MENU or GAME). */
        std::unordered_map<uint32_t, SceneState> clientScenes;

        /** @brief Maps player IDs to their (x, y) positions. */
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;

        /** @brief Maps obstacle IDs to their (x, y, width, height) values. */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles;

        /** @brief Maps projectile IDs to their (x, y, velX, velY, ownerId) data. */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, uint32_t>> projectiles;

        /** @brief Maps enemy IDs to their (x, y, velX, velY) data. */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> enemies;

        /** @brief Tracks damage cooldown timestamps for each player to prevent rapid consecutive hits. */
        std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> playerDamageCooldown;

        /** @brief Counter for assigning unique projectile IDs. */
        uint32_t nextProjectileId = 1;

        /** @brief Mutex for thread-safe access to shared game state. */
        std::mutex mtx;

        /** @brief Set of player IDs that have been eliminated. */
        std::unordered_set<uint32_t> deadPlayers;

        /** @brief Counter for assigning unique enemy IDs. */
        uint32_t nextEnemyId = 1;

        /** @brief Maps player IDs to their individual scores (kills achieved). */
        std::unordered_map<uint32_t, int> playerIndividualScores;

        /** @brief Global score (total kills across all players). */
        int totalScore = 0;

        /** @brief Maps enemy IDs to their behavior pattern identifiers. */
        std::unordered_map<uint32_t, std::string> enemyPatterns;

        /** @brief Cached entity references for all obstacles. */
        std::vector<ecs::entity_t> _obstacles;

        /** @brief Cached entity references for all enemies. */
        std::vector<ecs::entity_t> _enemies;

        /**
         * @brief Initializes starting positions for all players.
         */
        void initialize_player_positions();

        /**
         * @brief Indexes existing entities from the registry for quick access.
         */
        void index_existing_entities();

        /**
         * @brief Processes all pending messages from connected clients.
         */
        void process_pending_messages();

        /**
         * @brief Handles an incoming message from a client.
         * @param data Raw message buffer.
         * @param from UDP endpoint of the sending client.
         */
        void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

        /**
         * @brief Broadcasts updated player positions to all connected clients.
         */
        void broadcast_states_to_clients();

        /**
         * @brief Checks and handles collisions between players and enemies.
         */
        void check_player_enemy_collisions();

        /**
         * @brief Broadcasts a player death message to all clients.
         * @param clientId The ID of the deceased player.
         */
        void broadcast_player_death(uint32_t clientId);

        /**
         * @brief Initializes all obstacles in the game world.
         */
        void initialize_obstacles();

        /**
         * @brief Broadcasts an obstacle spawn message to all clients.
         * @param obstacleId Unique obstacle identifier.
         * @param x X-coordinate of the obstacle center.
         * @param y Y-coordinate of the obstacle center.
         * @param w Width of the obstacle.
         * @param h Height of the obstacle.
         */
        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);

        /**
         * @brief Broadcasts an obstacle despawn message to all clients.
         * @param obstacleId ID of the obstacle being removed.
         */
        void broadcast_obstacle_despawn(uint32_t obstacleId);

        /**
         * @brief Sleeps the current thread to maintain the target tick rate.
         * @param start Time point when the current tick started.
         * @param tick_ms Target tick duration in milliseconds.
         */
        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);

        /**
         * @brief Updates all projectiles' positions based on velocity.
         * @param dt Delta time in seconds.
         */
        void update_projectiles_server_only(float dt);

        /**
         * @brief Broadcasts current positions of all projectiles to all clients.
         */
        void broadcast_projectile_positions();

        /**
         * @brief Checks and handles collisions between projectiles and obstacles.
         */
        void check_projectile_collisions();

        /**
         * @brief Broadcasts a projectile spawn message to all clients.
         * @param projId Unique projectile identifier.
         * @param ownerId ID of the player who fired the projectile.
         * @param x Initial X-coordinate.
         * @param y Initial Y-coordinate.
         * @param vx Velocity in X direction.
         * @param vy Velocity in Y direction.
         */
        void broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId, float x, float y, float vx, float vy);

        /**
         * @brief Broadcasts a projectile despawn message to all clients.
         * @param projId ID of the projectile being removed.
         */
        void broadcast_projectile_despawn(uint32_t projId);

        /**
         * @brief Initializes all enemies with their starting positions and behaviors.
         */
        void initialize_enemies();

        /**
         * @brief Updates all enemies' positions based on their behavior patterns.
         * @param dt Delta time in seconds.
         */
        void update_enemies(float dt);

        /**
         * @brief Updates an enemy with default movement pattern.
         * @param id Enemy ID.
         * @param dt Delta time in seconds.
         */
        void update_enemy_default(uint32_t id, float dt);

        /**
         * @brief Updates an enemy with straight-line movement pattern.
         * @param id Enemy ID.
         * @param dt Delta time in seconds.
         */
        void update_enemy_straight(uint32_t id, float dt);

        /**
         * @brief Updates an enemy with zigzag movement pattern.
         * @param id Enemy ID.
         * @param dt Delta time in seconds.
         */
        void update_enemy_zigzag(uint32_t id, float dt);

        /**
         * @brief Updates an enemy with circular movement pattern.
         * @param id Enemy ID.
         * @param dt Delta time in seconds.
         */
        void update_enemy_circle(uint32_t id, float dt);

        /**
         * @brief Updates an enemy with turret/stationary behavior pattern.
         * @param id Enemy ID.
         * @param dt Delta time in seconds.
         */
        void update_enemy_turret(uint32_t id, float dt);

        /**
         * @brief Updates a boss enemy in phase 1 of its battle.
         * @param id Enemy ID.
         * @param dt Delta time in seconds.
         */
        void update_enemy_boss_phase1(uint32_t id, float dt);

        /**
         * @brief Broadcasts an enemy spawn message to all clients.
         * @param enemyId Unique enemy identifier.
         * @param x Starting X-coordinate.
         * @param y Starting Y-coordinate.
         * @param vx Initial velocity in X direction.
         * @param vy Initial velocity in Y direction.
         */
        void broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float vx, float vy);

        /**
         * @brief Broadcasts current positions of all enemies to all clients.
         */
        void broadcast_enemy_positions();

        /**
         * @brief Broadcasts an updated position for a specific enemy to all clients.
         * @param enemyId ID of the enemy.
         * @param x Current X-coordinate.
         * @param y Current Y-coordinate.
         */
        void broadcast_enemy_update(uint32_t enemyId, float x, float y);

        /**
         * @brief Broadcasts the current health values of all players to all clients.
         */
        void broadcast_player_health();

        /**
         * @brief Broadcasts the global cumulative score to all clients.
         */
        void broadcast_global_score();

        /**
         * @brief Broadcasts the individual scores of all players to all clients.
         */
        void broadcast_individual_scores();

        /**
         * @brief Broadcasts an enemy despawn message to all clients.
         * @param enemyId ID of the enemy being removed.
         */
        void broadcast_enemy_despawn(uint32_t enemyId);

        /**
         * @brief Checks and handles collisions between projectiles and enemies.
         */
        void check_projectile_enemy_collisions();

        /**
         * @brief Checks axis-aligned bounding box overlap between two rectangles.
         *
         * @param left1 Left edge of first rectangle.
         * @param right1 Right edge of first rectangle.
         * @param top1 Top edge of first rectangle.
         * @param bottom1 Bottom edge of first rectangle.
         * @param left2 Left edge of second rectangle.
         * @param right2 Right edge of second rectangle.
         * @param top2 Top edge of second rectangle.
         * @param bottom2 Bottom edge of second rectangle.
         * @return True if the rectangles overlap, false otherwise.
         */
        bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                        float left2, float right2, float top2, float bottom2);

        /**
         * @brief Checks if a player position would collide with obstacles.
         *
         * @param testX X-coordinate to test.
         * @param testY Y-coordinate to test.
         * @param playerWidth Width of the player hitbox.
         * @param playerHeight Height of the player hitbox.
         * @param obstacles Vector of obstacle entities to check against.
         * @return True if the position is blocked by an obstacle, false otherwise.
         */
        bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                        const std::vector<ecs::entity_t> &obstacles);
};