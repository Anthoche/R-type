/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** weapon
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a weapon entity in the game world.
     *
     * Weapons are items that can be picked up or equipped by players to attack enemies.
     * They can represent guns, swords, or any other offensive equipment.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the weapon in world coordinates.
     * @param y Vertical position of the weapon in world coordinates.
     * @param z Depth position of the weapon in world coordinates (default: 0.0).
     * @param width Width of the weapon (default: 20.0).
     * @param height Height of the weapon (default: 10.0).
     * @param depth Depth of the weapon (default: 5.0).
     * @param imagePath Path to the weapon's 2D sprite texture (default: empty string).
     * @param modelPath Path to the weapon's 3D model file (default: empty string).
     * @return The unique identifier of the created weapon entity.
     */
    ecs::entity_t create_weapon(ecs::registry &reg, float x, float y, float z = 0.f,
        float width = 20.f, float height = 10.f, float depth = 5.f,
        const std::string &imagePath = "", const std::string &modelPath = "");

}