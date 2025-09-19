/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#pragma once

#include "Network_handler/Include/UDP_socket.hpp"
#include "../Shared/protocol.hpp"
#include <thread>
#include <atomic>
#include <unordered_map>


class GameServer {
    UDP_socket socket;
    std::atomic<bool> gameStarted{false};
    uint32_t nextClientId = 1;
    // simple player state: position
    std::unordered_map<uint32_t, std::pair<float,float>> playerPositions;
    // enemies
    uint32_t nextEnemyId = 1;
    std::unordered_map<uint32_t, std::pair<float,float>> enemies; // id -> (x,y)
    float enemySpawnTimerSec = 0.f;
    // obstacles (static)
    std::unordered_map<uint32_t, std::tuple<float,float,float,float>> obstacles; // id -> (x,y,w,h)

    public:
        GameServer(uint16_t port);
        ~GameServer() = default;

        void run();

    private:
        void handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr);
        void broadcastGameStart();
        void game_loop();
        void handle_client_message(const std::vector<uint8_t>& data, const sockaddr_in& from);
        void initialize_player_positions();
        void process_pending_messages();
        void broadcast_states_to_clients();
        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
        void update_enemies(float dt);
        void spawn_enemy();
        void broadcast_enemy_spawn(uint32_t enemyId, float x, float y);
        void broadcast_enemy_update(uint32_t enemyId, float x, float y);
        void broadcast_enemy_despawn(uint32_t enemyId);
        // obstacles
        void initialize_obstacles();
        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);
        void broadcast_obstacle_despawn(uint32_t obstacleId);
};
