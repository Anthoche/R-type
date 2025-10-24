/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** checkpoint
*/

#pragma once
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

ecs::entity_t create_checkpoint(ecs::registry &reg, float x, float y, float z = 0.f,
    float width = 40.f, float height = 80.f, float depth = 40.f,
    const std::string &imagePath = "", const std::string &modelPath = "");

}
