/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** item
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates an item entity in the game world.
     *
     * Items are collectible objects that players can pick up.
     * They can represent various game objects such as keys, coins, or other interactive items.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the item in world coordinates.
     * @param y Vertical position of the item in world coordinates.
     * @param z Depth position of the item in world coordinates (default: 0.0).
     * @param width Width of the item (default: 20.0).
     * @param height Height of the item (default: 20.0).
     * @param depth Depth of the item (default: 20.0).
     * @param imagePath Path to the item's 2D sprite texture (default: empty string).
     * @param modelPath Path to the item's 3D model file (default: empty string).
     * @return The unique identifier of the created item entity.
     */
    ecs::entity_t create_item(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 20.f, float height = 20.f, float depth = 20.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}