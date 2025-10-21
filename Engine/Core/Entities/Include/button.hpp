/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** button
*/

#pragma once

#include <string>
#include <raylib.h>
#include "registry.hpp"
#include "entity.hpp"

namespace game::entities {

    /**
     * @brief Create an interactive button entity.
     *
     * @param reg ECS registry
     * @param id Unique button identifier
     * @param displayName Text displayed on the button
     * @param x X position
     * @param y Y position
     * @param z Z position (default 0.f)
     * @param width Button width
     * @param height Button height
     * @param color Background color
     * @param textColor Text color
     * @param fontSize Font size for the text
     * @return Created button entity
     */
    ecs::entity_t create_button(ecs::registry &reg, const std::string &id, const std::string &displayName,
        float x, float y, float z = 0.f, float width = 200.f, float height = 50.f,
        Color color = WHITE, Color textColor = BLACK, int fontSize = 35 );

}
