/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** gate
*/

#pragma once
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

ecs::entity_t create_gate(ecs::registry &reg, float x, float y, float z = 0.f,
    float width = 50.f, float height = 100.f, float depth = 50.f,
    const std::string &imagePath = "", const std::string &modelPath = "");

}
