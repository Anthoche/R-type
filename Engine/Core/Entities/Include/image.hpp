/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** image
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Create a static image entity at a 3D position.
     *
     * @param reg ECS registry
     * @param texture Texture2D to display
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate
     * @param width Image width
     * @param height Image height
     * @param depth Image depth (default = 0.f)
     * @return ecs::entity_t Handle to the new image entity
     */
    ecs::entity_t create_image(ecs::registry &reg,Texture2D texture, float x, float y, float z,
        float width, float height, float depth = 0.f
    );

    /**
     * @brief Create an image with a dynamic 3D position.
     *
     * @param reg ECS registry
     * @param texture Texture2D to display
     * @param pos Dynamic position reference
     * @param offset Position offset in 3D space
     * @param width Image width (default from texture)
     * @param height Image height (default from texture)
     * @param depth Image depth (default = 0.f)
     * @return ecs::entity_t Handle to the new image entity
     */
    ecs::entity_t create_image(ecs::registry &reg, Texture2D texture, DynamicPosition pos,
		Vector3 offset, float width = -1.f, float height = -1.f, float depth = 0.f
    );

} // namespace game::entities
