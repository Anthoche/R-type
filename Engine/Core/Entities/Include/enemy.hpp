/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** enemy
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Factory function to create an enemy entity with basic components.
     *
     * @param reg Reference to the ECS registry.
     * @param x Initial X position (default: 900.f)
     * @param y Initial Y position (default: 300.f)
     * @param z Initial Z position (default: 0.f)
     * @param imagePath Optional 2D sprite path
     * @param modelPath Optional 3D model path
     * @return The created enemy entity
     */
    ecs::entity_t create_enemy(ecs::registry &reg, float x = 900.f, float y = 300.f,
        float z = 0.f, const std::string &imagePath = "", const std::string &modelPath = "", const std::string &pattern = "");

    /**
     * @brief Sets up simple AI movement for enemies (e.g., move left).
     *
     * @param reg Reference to the ECS registry.
     */
    void setup_enemy_ai_system(ecs::registry &reg);

}
