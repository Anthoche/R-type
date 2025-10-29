/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** trap
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a trap entity in the game world.
     *
     * Traps are hazardous entities that deal damage to players or enemies upon contact.
     * They can be static or animated and are used to create challenging obstacles in levels.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the trap in world coordinates.
     * @param y Vertical position of the trap in world coordinates.
     * @param z Depth position of the trap in world coordinates (default: 0.0).
     * @param width Width of the trap (default: 30.0).
     * @param height Height of the trap (default: 30.0).
     * @param depth Depth of the trap (default: 30.0).
     * @param imagePath Path to the trap's 2D sprite texture (default: empty string).
     * @param modelPath Path to the trap's 3D model file (default: empty string).
     * @return The unique identifier of the created trap entity.
     */
    ecs::entity_t create_trap(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 30.f, float height = 30.f, float depth = 30.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}