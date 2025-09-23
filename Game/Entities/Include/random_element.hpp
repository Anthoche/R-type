/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** random element entity
*/

#pragma once

#include "components.hpp"
#include "../../Engine/Core/Include/registry.hpp"

namespace game::entities {

/**
 * Create a random_element that carries both an image (sprite) and a sound.
 */
ecs::entity_t create_random_element(ecs::registry &reg, float x, float y, float width, float height, const std::string &imagePath, const std::string &soundPath, float volume = 1.f, bool loop = false, bool autoplay = false);

}


