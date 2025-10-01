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
     *
     * The hitbox will mirror the owner's size from `collision_box` if present,
     * otherwise fallback to `drawable` dimensions. The hitbox position will be
     * kept in sync by the hitbox sync system.
     *
     * @param reg ECS registry
     * @param owner The owner entity the hitbox is associated with
     * @return ecs::entity_t The created hitbox entity
     */
    ecs::entity_t create_hitbox_for(ecs::registry &reg, ecs::entity_t owner);

    /**
     * @brief Register a system that synchronizes hitbox position/size with owner.
     *
     * Keeps `position` and `collision_box` aligned with the owner's components,
     * taking into account optional offsets stored in the `hitbox_link`.
     *
     * @param reg ECS registry
     */
    void setup_hitbox_sync_system(ecs::registry &reg);

} // namespace game::entities


