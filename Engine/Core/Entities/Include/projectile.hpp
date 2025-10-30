/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** projectile entity
*/

#pragma once
#include <stdint.h>
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a projectile entity with 3D position and velocity.
     *
     * @param reg ECS registry
     * @param x Initial X position
     * @param y Initial Y position
     * @param z Initial Z position (default 0.f)
     * @param vx Velocity along X
     * @param vy Velocity along Y
     * @param vz Velocity along Z (default 0.f)
     * @param ownerId Entity ID that fired this projectile
     * @return ecs::entity_t Handle to the projectile
     */
    ecs::entity_t create_projectile(ecs::registry &reg, float x, float y, float z,
        float vx, float vy, float vz, uint32_t ownerId);

}
