/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** decoration
*/

#pragma once
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a decorative entity in the game world.
     *
     * Decorations are non-interactive visual elements used to enhance the game's aesthetics.
     * They do not affect gameplay and are purely cosmetic.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the decoration in world coordinates.
     * @param y Vertical position of the decoration in world coordinates.
     * @param z Depth position of the decoration in world coordinates (default: 0.0).
     * @param width Width of the decoration (default: 50.0).
     * @param height Height of the decoration (default: 50.0).
     * @param depth Depth of the decoration (default: 50.0).
     * @param imagePath Path to the decoration's 2D sprite texture (default: empty string).
     * @param modelPath Path to the decoration's 3D model file (default: empty string).
     * @return The unique identifier of the created decoration entity.
     */
    ecs::entity_t create_decoration(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 50.f, float height = 50.f, float depth = 50.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}