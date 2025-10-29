/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** player
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Factory function to create a player entity with all necessary components.
     *
     * Initializes position, velocity, drawable/model, health, controllable, client_id, and hitbox.
     *
     * @param reg Reference to the ECS registry.
     * @param x Initial X position (default: 100.f).
     * @param y Initial Y position (default: 0.f).
     * @param z Initial Z position (default: 0.f).
     * @param imagePath Optional 2D sprite path.
     * @param modelPath Optional 3D model path.
     * @param clientId Client ID (for multiplayer).
     * @return The created player entity ID.
     */
    ecs::entity_t create_player(ecs::registry &reg, float x = 100.f, float y = 0.f, float z = 0.f,
        const std::string &imagePath = "", const std::string &modelPath = "", uint32_t clientId = 0);

    /**
     * @brief Setup the player control system.
     * Reads input and updates player velocity.
     */
    void setup_player_control_system(ecs::registry &reg);

    /**
     * @brief Constrain player movement within screen/world bounds.
     */
    void setup_player_bounds_system(ecs::registry &reg, float screen_width, float screen_height, float screen_depth);
}
