/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** text entity
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {
	/**
	* Create a text entity at a position.
	*/
	ecs::entity_t create_text(ecs::registry &reg, Vector2 pos, const std::string &content, Color color, int fontSize);

	/**
	* Create a text entity at a position, with advanced parameters.
	*/
	ecs::entity_t create_text(ecs::registry &reg, Vector2 pos, const std::string &content, Color color = WHITE, float spacing = 1.f,
							int fontSize = 24, Font const &font = GetFontDefault());

	/**
	 * Create a text at a dynamic position
	 */
	ecs::entity_t create_text(ecs::registry &reg, DynamicPosition pos, Vector2 offset, const std::string &content, Color color,
							float spacing, int fontSize, Font const &font);
}