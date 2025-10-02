/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** button
*/

#include "Include/button.hpp"
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {
	ecs::entity_t create_button(ecs::registry &reg, std::string const &id, std::string const &displayName, Vector2 pos, Vector2 size,
								Color color, Color textColor) {
		auto button = reg.spawn_entity();

		reg.emplace_component<component::position>(button, pos.x, pos.y);
		reg.emplace_component<component::clickable>(button, id);
		reg.emplace_component<component::hoverable>(button, id);
		reg.emplace_component<component::type>(button, component::entity_type::BUTTON);

		component::drawable drawable;
		drawable.width = size.x;
		drawable.height = size.y;
		drawable.color = color;
		reg.add_component<component::drawable>(button, std::move(drawable));

		component::text text;
		text.spacing = -1;
		text.font_size = 35;
		text.content = displayName;
		text.color = textColor;
		reg.emplace_component<component::text>(button, std::move(text));
		return button;
	}
}
