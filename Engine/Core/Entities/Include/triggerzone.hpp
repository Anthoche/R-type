/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TriggerZone
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a trigger zone entity in the game world.
     *
     * Trigger zones are invisible areas that detect when entities enter or exit them.
     * They are used to activate events, cutscenes, checkpoints, or other gameplay mechanics
     * when a player or entity enters the zone.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param x Horizontal position of the trigger zone in world coordinates.
     * @param y Vertical position of the trigger zone in world coordinates.
     * @param z Depth position of the trigger zone in world coordinates (default: 0.0).
     * @param width Width of the trigger zone (default: 100.0).
     * @param height Height of the trigger zone (default: 100.0).
     * @param depth Depth of the trigger zone (default: 100.0).
     * @param id Unique identifier for the trigger zone (default: empty string).
     * @return The unique identifier of the created trigger zone entity.
     */
    ecs::entity_t create_triggerzone(ecs::registry &reg,float x, float y, float z = 0.f,
        float width = 100.f, float height = 100.f, float depth = 100.f, const std::string &id = "");

}