/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** projectile
*/

#pragma once

#include <stdint.h>
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a projectile entity with the required components.
     * @param reg Reference to the ECS registry.
     * @param x Initial X coordinate.
     * @param y Initial Y coordinate.
     * @param vx Initial horizontal velocity.
     * @param vy Initial vertical velocity.
     * @param ownerId ID of the entity (player or enemy) that fired this projectile.
     * @return ecs::entity_t Handle to the newly created projectile entity.
     */
    ecs::entity_t create_projectile(ecs::registry &reg, float x, float y, float vx, float vy, uint32_t ownerId);

} // namespace game::entities
