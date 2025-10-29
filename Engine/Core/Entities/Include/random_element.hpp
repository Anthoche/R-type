/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** random_element
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Create a random_element with a 3D position, optional sprite and sound.
     *
     * @param reg ECS registry
     * @param x X coordinate
     * @param y Y coordinate
     * @param z Z coordinate (default 0.f)
     * @param width Width of the element
     * @param height Height of the element
     * @param depth Depth of the element (default same as height)
     * @param imagePath Path to sprite image
     * @param soundPath Path to audio file
     * @param volume Audio volume
     * @param loop Loop audio flag
     * @param autoplay Play audio on spawn
     * @return ecs::entity_t Handle to the element
     */
    ecs::entity_t create_random_element(ecs::registry &reg, float x, float y, float z,
        float width, float height, float depth, const std::string &imagePath,
        const std::string &soundPath, float volume = 1.f, bool loop = false, bool autoplay = false);

}
