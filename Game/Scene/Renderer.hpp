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
#include "../../Engine/Rendering/Raylib.hpp"

namespace game::render {

    inline Color colorForPlayerId(uint32_t id) {
        static Color palette[] = { RAYWHITE, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, GOLD, LIME, SKYBLUE };
        return palette[id % (sizeof(palette)/sizeof(palette[0]))];
    }

    inline void drawObstacles(Raylib &rl) {
        rl.drawRectangle(200 - 30, 400 - 30, 60, 60, GRAY);
        rl.drawRectangle(400 - 30, 400 - 30, 60, 60, GRAY);
        rl.drawRectangle(600 - 30, 400 - 30, 60, 60, GRAY);
    }

    inline void drawPlayers(Raylib &rl, const std::unordered_map<uint32_t, std::pair<float,float>> &players) {
        for (auto const &kv : players) {
            float x = kv.second.first;
            float y = kv.second.second;
            rl.drawRectangle((int)(x - 15), (int)(y - 15), 30, 30, colorForPlayerId(kv.first));
        }
    }

    inline void drawEnemies(Raylib &rl, const std::unordered_map<uint32_t, std::pair<float,float>> &enemies) {
        for (auto const &kv : enemies) {
            float x = kv.second.first;
            float y = kv.second.second;
            rl.drawRectangle((int)(x - 20), (int)(y - 20), 40, 40, RED);
        }
    }
}


