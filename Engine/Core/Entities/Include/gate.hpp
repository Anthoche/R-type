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

    /**
     * @brief Creates a gate entity in the game world.
     *
     * A gate is a barrier or door that can block or allow passage.
     * Gates are typically used to control access to different areas of the game.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the gate in world coordinates.
     * @param y Vertical position of the gate in world coordinates.
     * @param z Depth position of the gate in world coordinates (default: 0.0).
     * @param width Width of the gate (default: 50.0).
     * @param height Height of the gate (default: 100.0).
     * @param depth Depth of the gate (default: 50.0).
     * @param imagePath Path to the gate's 2D sprite texture (default: empty string).
     * @param modelPath Path to the gate's 3D model file (default: empty string).
     * @return The unique identifier of the created gate entity.
     */
    ecs::entity_t create_gate(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 50.f, float height = 100.f, float depth = 50.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}