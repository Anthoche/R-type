/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** png
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a PNG image entity in the game world.
     *
     * PNG entities are used to display static images with transparency support.
     * They are typically used for UI elements, icons, or background images.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the image in world coordinates.
     * @param y Vertical position of the image in world coordinates.
     * @param z Depth position of the image in world coordinates (default: 0.0).
     * @param width Width of the image (default: 50.0).
     * @param height Height of the image (default: 50.0).
     * @param depth Depth of the image (default: 1.0).
     * @param imagePath Path to the PNG image file (default: empty string).
     * @param modelPath Path to an optional 3D model file (default: empty string).
     * @return The unique identifier of the created PNG entity.
     */
    ecs::entity_t create_png(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 50.f, float height = 50.f, float depth = 1.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}