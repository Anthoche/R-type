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

class ServerGame {
    public:
        ServerGame(Connexion &conn);
        void run();
        void load_players(const std::string &path);
        void load_level(const std::string &path);
        void broadcast_full_registry_to(uint32_t clientId);

    private:
        Connexion &connexion;
        ecs::registry registry_server;
        std::unordered_map<uint32_t, SceneState> clientScenes;
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles;
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, uint32_t>> projectiles;
        std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> playerDamageCooldown;
        uint32_t nextProjectileId = 1;
        std::mutex mtx;
        std::unordered_set<uint32_t> deadPlayers;
        uint32_t nextEnemyId = 1;
        std::unordered_map<uint32_t, int> playerIndividualScores;
        int totalScore = 0;
        std::unordered_map<uint32_t, std::string> enemyPatterns;
        std::vector<ecs::entity_t> _obstacles;
        std::vector<ecs::entity_t> _enemies;

        void initialize_player_positions();
        void index_existing_entities();
        void process_pending_messages();
        void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);
        void broadcast_states_to_clients();
        void check_player_enemy_collisions();
        void broadcast_player_death(uint32_t clientId);
        void initialize_obstacles();
        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);
        void broadcast_obstacle_despawn(uint32_t obstacleId);
        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
        void update_projectiles_server_only(float dt);
        void broadcast_projectile_positions();
        void check_projectile_collisions();
        void broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId, float x, float y, float vx, float vy);
        void broadcast_projectile_despawn(uint32_t projId);
        void initialize_enemies();
        void update_enemies(float dt);
        void update_enemy_default(uint32_t id, float dt);
        void update_enemy_straight(uint32_t id, float dt);
        void update_enemy_zigzag(uint32_t id, float dt);
        void update_enemy_circle(uint32_t id, float dt);
        void update_enemy_turret(uint32_t id, float dt);
        void update_enemy_boss_phase1(uint32_t id, float dt);
        void broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float vx, float vy);
        void broadcast_enemy_positions();
        void broadcast_enemy_update(uint32_t enemyId, float x, float y);

        /**
         * @brief Broadcasts the current health of all players to all clients.
         */
        void broadcast_player_health();

        /**
         * @brief Broadcasts the current global score to all clients.
         */
        void broadcast_global_score();

        /**
         * @brief Broadcasts the individual scores of all players to all clients.
         */
        void broadcast_individual_scores();

        void broadcast_enemy_despawn(uint32_t enemyId);
        void check_projectile_enemy_collisions();
        bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                        float left2, float right2, float top2, float bottom2);
        bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                        const std::vector<ecs::entity_t> &obstacles);
};
