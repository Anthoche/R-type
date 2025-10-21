/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** item
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

ecs::entity_t create_item(ecs::registry &reg, float x, float y, float z = 0.f,
    float width = 20.f, float height = 20.f, float depth = 20.f,
    const std::string &imagePath = "", const std::string &modelPath = "");

}
