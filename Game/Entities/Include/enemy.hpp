/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** enemy
*/

#pragma once

#include "../../Engine/Core/Include/registry.hpp"
#include "../../Engine/Core/Include/entity.hpp"
#include "components.hpp"

namespace game::entities {

    /**
     * @brief Factory function to create an enemy entity with basic components.
     *
     * @param reg Reference to the ECS registry.
     * @param x Initial X position.
     * @param y Initial Y position.
     * @return The created enemy entity.
     */
    ecs::entity_t create_enemy(ecs::registry &reg, float x = 900.f, float y = 300.f);

    /**
     * @brief Sets up simple AI movement for enemies (e.g., move left).
     *
     * @param reg Reference to the ECS registry.
     */
    void setup_enemy_ai_system(ecs::registry &reg);

} // namespace game::entities


