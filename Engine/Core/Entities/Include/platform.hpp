/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** platform
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a platform entity in the game world.
     *
     * Platforms are solid surfaces that players can stand on or collide with.
     * They can be static or moving, and are commonly used for level design and navigation.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the platform in world coordinates.
     * @param y Vertical position of the platform in world coordinates.
     * @param z Depth position of the platform in world coordinates (default: 0.0).
     * @param width Width of the platform (default: 100.0).
     * @param height Height of the platform (default: 20.0).
     * @param depth Depth of the platform (default: 20.0).
     * @param imagePath Path to the platform's 2D sprite texture (default: empty string).
     * @param modelPath Path to the platform's 3D model file (default: empty string).
     * @return The unique identifier of the created platform entity.
     */
    ecs::entity_t create_platform(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 100.f, float height = 20.f, float depth = 20.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}