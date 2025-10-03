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
#include <nlohmann/json.hpp>
#include "../../../Shared/protocol.hpp"
#include "../../Network_handler/Include/connexion.hpp"
#include "../../../Engine/Core/Include/registry.hpp"
#include "../../../Engine/Utils/Include/entity_storage.hpp"

class ServerGame {
    Connexion &connexion; ///< Reference to the UDP connection manager
    std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Player positions keyed by player ID.
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles; ///< Obstacles keyed by ID (x,y,w,h).

public:
    /**
     * @brief Construct a new ServerGame.
     * @param conn Reference to the network connection manager.
     */
    explicit ServerGame(Connexion &conn);

    /**
     * @brief Run the main server game loop.
     *
     * Processes client messages, updates game state, and broadcasts updates.
     */
    void run();

private:
    void initialize_player_positions();
    void initialize_obstacles();
    void setup_async_receive();

    void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

    void broadcast_states_to_clients();
    void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);
    void broadcast_obstacle_despawn(uint32_t obstacleId);

    void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};