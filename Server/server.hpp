/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/
#pragma once

#include "Network_handler/Include/connexion.hpp"
#include "../Shared/protocol.hpp"
#include <asio.hpp>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <tuple>
#include <iostream>
#include <algorithm>

/**
 * @brief Main server class for the R-Type multiplayer game.
 *
 * Handles client connections, game state, and network messages.
 * Manages up to 4 players, enemy spawning, and obstacle placement.
 */
class GameServer {
    asio::io_context ioContext; ///< Asio I/O context
    Connexion connexion; ///< Network connection manager
    std::atomic<bool> gameStarted{false}; ///< Flag indicating if the game has started
    uint32_t nextClientId = 1; ///< Next available client ID
    std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< player positions
    uint32_t nextEnemyId = 1; ///< Next enemy ID
    std::unordered_map<uint32_t, std::pair<float, float>> enemies;
    float enemySpawnTimerSec = 0.f;
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles;

public:
    GameServer(uint16_t port);

    /**
     * @brief Main server loop: wait for clients, then run the game.
     */
    void run();

    ~GameServer() = default;

private:
    void handleClientHello(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& clientEndpoint);
    void broadcastGameStart();
    void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);
    void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};
