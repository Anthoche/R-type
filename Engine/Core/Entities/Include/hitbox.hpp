/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** hitbox
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Create a generic hitbox entity associated to an owner entity.
     * The hitbox mirrors the owner's collision_box or drawable size.
     * Position and size are kept in sync via setup_hitbox_sync_system.
     */
    ecs::entity_t create_hitbox_for(ecs::registry &reg, ecs::entity_t owner);

    /**
     * @brief Register a system that synchronizes hitbox position and size with owner.
     * Works in 3D space (x, y, z + width, height, depth).
     */
    void setup_hitbox_sync_system(ecs::registry &reg);

} // namespace game::entities
