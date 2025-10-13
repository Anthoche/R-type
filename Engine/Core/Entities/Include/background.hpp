/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** background entity
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

/**
 * Create a background entity with an optional sprite image.
 */
ecs::entity_t create_background(ecs::registry &reg, float x, float y, float width, float height, const std::string &imagePath = "", float scale = 1.f);

}


