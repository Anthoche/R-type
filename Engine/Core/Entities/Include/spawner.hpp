/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** spawner
*/

#pragma once
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

ecs::entity_t create_spawner(ecs::registry &reg, float x, float y, float z = 0.f,
    float width = 30.f, float height = 30.f, float depth = 30.f,
    const std::string &imagePath = "", const std::string &modelPath = "");

}
