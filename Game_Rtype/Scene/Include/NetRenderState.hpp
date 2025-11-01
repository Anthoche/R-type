/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** NetRenderState
*/

#pragma once

#include <unordered_map>
#include <utility>
#include <mutex>
#include <cstdint>

/**
 * @struct NetRenderState
 * @brief Thread-safe structure storing render-related network state.
 *
 * This structure is shared between the networking layer and the rendering layer.
 * It allows safe concurrent access to the current game state received from the server.
 *
 * - Stores player and enemy positions by their unique IDs.
 * - Uses a mutex to synchronize access between threads.
 */
struct NetRenderState {
    std::mutex mutex; ///< Mutex for synchronizing access across threads.

    /**
     * @brief Player positions mapped by player ID.
     * 
     * Each entry maps a player ID to their position (x, y).
     */
    std::unordered_map<uint32_t, std::pair<float,float>> players;

    /**
     * @brief Enemy positions mapped by enemy ID.
     * 
     * Each entry maps an enemy ID to their position (x, y).
     */
    std::unordered_map<uint32_t, std::pair<float,float>> enemies;
};
