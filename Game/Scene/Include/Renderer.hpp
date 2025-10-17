/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Renderer helpers for network-driven client
*/

#pragma once

#include <unordered_map>
#include <cstdint>
#include <utility>
#include "../../../Engine/Rendering/Raylib.hpp"

namespace game::render {

    /**
     * @brief Get a color associated with a player ID.
     *
     * Uses a fixed palette of colors and assigns one deterministically
     * based on the player ID modulo the palette size.
     *
     * @param id The unique player ID.
     * @return Color associated with this player.
     */
    inline Color colorForPlayerId(uint32_t id) {
        static Color palette[] = {
            RAYWHITE, BLUE, GREEN, YELLOW, ORANGE,
            PURPLE, PINK, GOLD, LIME, SKYBLUE
        };
        return palette[id % (sizeof(palette) / sizeof(palette[0]))];
    }

    /**
     * @brief Draws static obstacles on the map.
     *
     * This is a placeholder implementation: three gray blocks at
     * fixed positions. Can be extended to support dynamic obstacles.
     *
     * @param rl Reference to the Raylib wrapper for rendering.
     */
    inline void drawObstacles(Raylib &rl) {
        rl.drawRectangle(200 - 30, 400 - 30, 60, 60, GRAY);
        rl.drawRectangle(400 - 30, 400 - 30, 60, 60, GRAY);
        rl.drawRectangle(600 - 30, 400 - 30, 60, 60, GRAY);
    }

    /**
     * @brief Draws all players at their positions.
     *
     * Each player is represented as a colored square. The color is chosen
     * deterministically based on their ID (see colorForPlayerId()).
     *
     * @param rl Reference to the Raylib wrapper for rendering.
     * @param players Map of player IDs to positions (x, y).
     */
    inline void drawPlayers(Raylib &rl, const std::unordered_map<uint32_t, std::pair<float,float>> &players) {
        for (auto const &kv : players) {
            float x = kv.second.first;
            float y = kv.second.second;
            rl.drawRectangle((int)(x - 15), (int)(y - 15), 30, 30, colorForPlayerId(kv.first));
        }
    }

    /**
     * @brief Draws all enemies at their positions.
     *
     * Enemies are represented as red squares slightly larger than players.
     *
     * @param rl Reference to the Raylib wrapper for rendering.
     * @param enemies Map of enemy IDs to positions (x, y).
     */
    inline void drawEnemies(Raylib &rl, const std::unordered_map<uint32_t, std::pair<float,float>> &enemies) {
        for (auto const &kv : enemies) {
            float x = kv.second.first;
            float y = kv.second.second;
            rl.drawRectangle((int)(x - 20), (int)(y - 20), 40, 40, RED);
        }
    }
}
