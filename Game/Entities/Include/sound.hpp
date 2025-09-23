/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sound entity
*/

#pragma once

#include "components.hpp"
#include "../../Engine/Core/Include/registry.hpp"

namespace game::entities {

/**
 * Create a sound entity configured to play a sound or music.
 */
ecs::entity_t create_sound(ecs::registry &reg, const std::string &soundPath, float volume = 1.f, bool loop = false, bool autoplay = true);

}


