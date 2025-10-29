/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** powerup
*/

#pragma once
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a power-up entity in the game world.
     *
     * Power-ups are collectible items that provide temporary or permanent bonuses to the player,
     * such as increased damage, speed boosts, shields, or special abilities.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the power-up in world coordinates.
     * @param y Vertical position of the power-up in world coordinates.
     * @param z Depth position of the power-up in world coordinates (default: 0.0).
     * @param width Width of the power-up (default: 20.0).
     * @param height Height of the power-up (default: 20.0).
     * @param depth Depth of the power-up (default: 20.0).
     * @param imagePath Path to the power-up's 2D sprite texture (default: empty string).
     * @param modelPath Path to the power-up's 3D model file (default: empty string).
     * @return The unique identifier of the created power-up entity.
     */
    ecs::entity_t create_powerup(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 20.f, float height = 20.f, float depth = 20.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}