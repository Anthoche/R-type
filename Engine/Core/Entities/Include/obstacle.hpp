/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** obstacle
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Factory function to create a static obstacle entity.
     *
     * @param reg Reference to the ECS registry.
     * @param x Initial X position.
     * @param y Initial Y position.
     * @return The created obstacle entity.
     */
    ecs::entity_t create_obstacle(ecs::registry &reg, float x, float y, const std::string &imagePath = "");

} // namespace game::entities


