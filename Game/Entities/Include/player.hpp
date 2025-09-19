/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** player
*/

#pragma once

#include "../../Engine/Core/Include/registry.hpp"
#include "../../Engine/Core/Include/entity.hpp"
#include "components.hpp"

namespace game::entities {

    /**
     * @brief Factory function to create a player entity with all necessary components.
     *
     * Initializes components such as position, velocity, drawable, health, controllable, etc.
     *
     * @param reg Reference to the ECS registry.
     * @param x Initial X position (default: 100.f).
     * @param y Initial Y position (default: 300.f).
     * @return The created player entity.
     */
    ecs::entity_t create_player(ecs::registry &reg, float x = 100.f, float y = 300.f);

    /**
     * @brief Sets up the player control system.
     *
     * This system reads player input and updates the player's velocity accordingly.
     *
     * @param reg Reference to the ECS registry.
     */
    void setup_player_control_system(ecs::registry &reg);

    /**
     * @brief Sets up a system to constrain the player's movement within the screen boundaries.
     *
     * Ensures that the player cannot move outside the visible area of the game.
     *
     * @param reg Reference to the ECS registry.
     * @param screen_width Width of the game screen.
     * @param screen_height Height of the game screen.
     */
    void setup_player_bounds_system(ecs::registry &reg, float screen_width, float screen_height);

} // namespace game::entities
