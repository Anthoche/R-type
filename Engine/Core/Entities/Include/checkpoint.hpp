/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** checkpoint
*/

#pragma once
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a checkpoint entity in the game world.
     *
     * A checkpoint represents a progress marker or save point in the game.
     * This function creates an entity with position, size, and optional visual assets.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the checkpoint in world coordinates.
     * @param y Vertical position of the checkpoint in world coordinates.
     * @param z Depth position of the checkpoint in world coordinates (default: 0.0).
     * @param width Width of the checkpoint's collision box (default: 40.0).
     * @param height Height of the checkpoint's collision box (default: 80.0).
     * @param depth Depth of the checkpoint's collision box (default: 40.0).
     * @param imagePath Path to the checkpoint's 2D sprite texture (default: empty string).
     * @param modelPath Path to the checkpoint's 3D model file (default: empty string).
     * @return The unique identifier of the created checkpoint entity.
     */
    ecs::entity_t create_checkpoint(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 40.f, float height = 80.f, float depth = 40.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}