/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** background
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

/**
 * @brief Create a background entity with optional sprite or 3D model.
 *
 * @param reg ECS registry
 * @param x Initial X position
 * @param y Initial Y position
 * @param z Initial Z position (default 0.f)
 * @param width Width of the background
 * @param height Height of the background
 * @param depth Depth of the background (default 1.f)
 * @param imagePath Optional 2D sprite path
 * @param modelPath Optional 3D model path
 * @param scale Scale factor for sprite or model
 * @return Created entity ID
 */
ecs::entity_t create_background(ecs::registry &reg, float x, float y, float z = 0.f,
    float width = 800.f,float height = 600.f, float depth = 1.f,
    const std::string &imagePath = "", const std::string &modelPath = "", float scale = 1.f);

}
