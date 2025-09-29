/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** text entity
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

/**
 * Create a text entity at a position, with optional font.
 */
ecs::entity_t create_text(ecs::registry &reg, float x, float y, const std::string &content, int fontSize = 24, const std::string &fontPath = "");

}


