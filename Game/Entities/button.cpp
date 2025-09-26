/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** button
*/

#include "button.hpp"

#include "components.hpp"

namespace game::entities {
	ecs::entity_t create_button(ecs::registry &reg, std::string const &id, std::string const &displayName, Vector2 pos, Vector2 size, Color color, Color textColor) {
		auto button = reg.spawn_entity();

		reg.emplace_component<component::position>(button, pos.x, pos.y);
		reg.emplace_component<component::clickable>(button, id);
		component::drawable drawable;
		drawable.width = size.x;
		drawable.height = size.y;
		drawable.r = 0.f; drawable.g = 1.f; drawable.b = 1.f; drawable.a = 1.f;
		reg.add_component<component::drawable>(button, std::move(drawable));

		//TODO: do something with displayName, color & textColor
		return button;
	}
}
