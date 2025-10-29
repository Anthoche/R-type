/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** spawner
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

    namespace game::entities {

    /**
     * @brief Creates a spawner entity in the game world.
     *
     * Spawners are entities that generate other entities at their location.
     * They are commonly used to create enemies, items, or projectiles at specific intervals or conditions.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the spawner in world coordinates.
     * @param y Vertical position of the spawner in world coordinates.
     * @param z Depth position of the spawner in world coordinates (default: 0.0).
     * @param width Width of the spawner's area (default: 30.0).
     * @param height Height of the spawner's area (default: 30.0).
     * @param depth Depth of the spawner's area (default: 30.0).
     * @param imagePath Path to the spawner's 2D sprite texture (default: empty string).
     * @param modelPath Path to the spawner's 3D model file (default: empty string).
     * @return The unique identifier of the created spawner entity.
     */
    ecs::entity_t create_spawner(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 30.f, float height = 30.f, float depth = 30.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}
