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

class ServerGame {
    UDP_socket &socket;
    // players state
    std::unordered_map<uint32_t, std::pair<float,float>> playerPositions;
    // enemies
    uint32_t nextEnemyId = 1;
    std::unordered_map<uint32_t, std::pair<float,float>> enemies; // id -> (x,y)
    float enemySpawnTimerSec = 0.f;
    // obstacles
    std::unordered_map<uint32_t, std::tuple<float,float,float,float>> obstacles; // id -> (x,y,w,h)

public:
    explicit ServerGame(UDP_socket &sock);
    void run();

private:
    void initialize_player_positions();
    void initialize_obstacles();
    void process_pending_messages();
    void handle_client_message(const std::vector<uint8_t>& data, const sockaddr_in& from);
    void update_enemies(float dt);
    void spawn_enemy();
    void broadcast_states_to_clients();
    void broadcast_enemy_spawn(uint32_t enemyId, float x, float y);
    void broadcast_enemy_update(uint32_t enemyId, float x, float y);
    void broadcast_enemy_despawn(uint32_t enemyId);
    void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);
    void broadcast_obstacle_despawn(uint32_t obstacleId);
    void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};


