/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/

#pragma once

#include "../../Server/Include/IServerGame.hpp"
#include <asio/ip/udp.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <mutex>
#include <vector>

/**
 * @class ServerGame
 * @brief Implementation of IServerGame for R-Type multiplayer gameplay.
 *
 * This class handles:
 * - The main server game loop (~60 FPS).
 * - Player management, movement, shooting, and death.
 * - Enemy spawning, movement patterns, and AI projectiles.
 * - Collision detection between all entities.
 * - Broadcasting of all gameplay updates to clients.
 */
class ServerGame : public IServerGame {
    public:
        /**
         * @brief Constructs a ServerGame instance.
         * @param conn Reference to the Connexion handler for network communication.
         */
        explicit ServerGame(Connexion &conn);

        /**
         * @brief Main server game loop.
         */
        void run() override;

        /**
         * @brief Loads player data from a JSON configuration file.
         */
        void load_players(const std::string &path);

        /**
         * @brief Loads level entities from a JSON configuration file.
         */
        void load_level(const std::string &path);

        /**
         * @brief Sends the complete game registry state to a specific client.
         */
        void broadcast_full_registry_to(uint32_t clientId);

    private:

        static constexpr int MAX_LEVELS = 3;
        bool _isEndless = false;
        bool gameCompleted = false;
        /** @brief Maps enemy projectile IDs to their (x,y,z,velX,velY,velZ,ownerId). */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, uint32_t>> enemyProjectiles;

        /** @brief Counter for assigning unique enemy projectile IDs. */
        uint32_t nextEnemyProjectileId = 100000;

        /** @brief Reference to the network connection handler. */
        Connexion &connexion;

        /** @brief ECS registry containing all server-side game entities and components. */
        ecs::registry registry_server;

        /** @brief Maps client IDs to their current scene state (MENU or GAME). */
        std::unordered_map<uint32_t, SceneState> clientScenes;

        /** @brief Maps player IDs to their (x, y) positions. Note: keeping 2D for backward compat, add z if needed */
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;

        /** @brief Maps obstacle IDs to their (x, y, z, width, height, depth). */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float>> obstacles;

        /** @brief Maps projectile IDs to their (x, y, z, velX, velY, velZ, ownerId). */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, uint32_t>> projectiles;

        /** @brief Cooldown timestamps to avoid damage spam. */
        std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> playerDamageCooldown;

        struct InputEvent {
            InputCode code;
            bool pressed;
        };

        struct PlayerInputState {
            bool up{false};
            bool down{false};
            bool left{false};
            bool right{false};
        };

        std::unordered_map<uint32_t, std::vector<InputEvent>> playerInputBuffers;
        std::unordered_map<uint32_t, PlayerInputState> playerInputStates;

        /** @brief Counter for assigning unique projectile IDs. */
        uint32_t nextProjectileId = 1;

        /** @brief Mutex for thread-safe access. */
        std::mutex mtx;

        /** @brief Set of dead player IDs. */
        std::unordered_set<uint32_t> deadPlayers;

        /** @brief Counter for assigning unique enemy IDs. */
        uint32_t nextEnemyId = 1;

        /** @brief Individual scores per player. */
        std::unordered_map<uint32_t, int> playerIndividualScores;

        /** @brief Total cumulative score. */
        int totalScore = 0;

        /** @brief Enemy movement pattern registry. */
        std::unordered_map<uint32_t, std::string> enemyPatterns;

        /** @brief Cached references for obstacles. */
        std::vector<ecs::entity_t> _obstacles;

        /** @brief Cached references for enemies. */
        std::vector<ecs::entity_t> _enemies;

         int currentLevel = 1;
        bool levelTransitionPending = false;
        std::chrono::steady_clock::time_point levelTransitionTime;
        const float LEVEL_TRANSITION_DELAY = 6.0f;



        void initialize_player_positions();
        void index_existing_entities();

        void process_pending_messages();
        void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);
        void broadcast_states_to_clients();
        void check_player_enemy_collisions();
        void broadcast_player_death(uint32_t clientId);
        void initialize_obstacles();

        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float z, float w, float h, float d, float vx, float vy, float vz);
        void broadcast_obstacle_positions();
        void broadcast_obstacle_update(uint32_t obstacleId, float x, float y, float z,
                                          float vx, float vy, float vz);
        void broadcast_obstacle_despawn(uint32_t obstacleId);

        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);

        void update_projectiles_server_only(float dt);
        void broadcast_projectile_positions();
        void check_projectile_collisions();
        void broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId, float x, float y, float z, float vx, float vy, float vz);
        void broadcast_projectile_despawn(uint32_t projId);

        void update_enemies(float dt);
        void update_enemy_default(uint32_t id, float dt);
        void update_enemy_straight(uint32_t id, float dt);
        void update_enemy_zigzag(uint32_t id, float dt);
        void update_enemy_circle(uint32_t id, float dt);
        void update_enemy_turret(uint32_t id, float dt, float rapidfire);
        void update_enemy_boss_phase1(uint32_t id, float dt);
        void update_enemy_figure8(uint32_t id, float dt);
        void update_enemy_spiral(uint32_t id, float dt);
        void update_obstacles(float dt);

        void broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float z, float vx, float vy, float vz, float width, float height);
        void broadcast_enemy_positions();
        void broadcast_enemy_update(uint32_t enemyId, float x, float y, float z);
        void broadcast_enemy_despawn(uint32_t enemyId);
        void broadcast_boss_death(uint32_t bossId);

        void shoot_enemy_projectile(uint32_t enemyId, float x, float y, float vx, float vy);
        void update_enemy_projectiles_server_only(float dt);
        void check_enemy_projectile_player_collisions();

        void broadcast_enemy_projectile_spawn(uint32_t projId, uint32_t ownerId, float x, float y, float z, float vx, float vy, float vz);
        void broadcast_enemy_projectile_positions();
        void broadcast_enemy_projectile_despawn(uint32_t projId);

        void broadcast_player_health();
        void broadcast_global_score();
        void broadcast_endless_mode(bool isEndless);
        void broadcast_global_health(int16_t health);
        void broadcast_individual_scores();
        void check_projectile_enemy_collisions();

        void load_next_level();
        void clear_level_entities();

        void process_player_inputs(float dt);
        bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                                float left2, float right2, float top2, float bottom2);

        bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                                const std::vector<ecs::entity_t> &obstacles);
};