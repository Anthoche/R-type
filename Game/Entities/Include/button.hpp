/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** button
*/

#ifndef BUTTON_HPP
	#define BUTTON_HPP

#include <string>
#include <raylib.h>
#include "registry.hpp"
#include "entity.hpp"

namespace game::entities {
	ecs::entity_t create_button(ecs::registry &reg, std::string const &id, std::string const &displayName, Vector2 pos, Vector2 size, Color color, Color textColor);
}

#endif