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

struct NetRenderState {
    std::mutex mutex;
    std::unordered_map<uint32_t, std::pair<float,float>> players; // id -> (x,y)
    std::unordered_map<uint32_t, std::pair<float,float>> enemies; // id -> (x,y)
};


