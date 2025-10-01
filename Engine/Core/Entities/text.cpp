/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** text entity impl
*/

#include "Include/text.hpp"

namespace game::entities {

	ecs::entity_t create_text(ecs::registry &reg, Vector2 pos, const std::string &content, Color color, int fontSize) {
		auto e = reg.spawn_entity();
		reg.emplace_component<component::position>(e, pos.x, pos.y);

		component::text t;
		t.content = content;
		t.font_size = fontSize;
		t.color = color;
		reg.add_component<component::text>(e, std::move(t));

		component::drawable drawable;
		reg.add_component<component::drawable>(e, std::move(drawable));

		reg.emplace_component<component::type>(e, component::entity_type::TEXT);
		return e;
	}

	ecs::entity_t create_text(ecs::registry &reg, Vector2 pos, const std::string &content, Color color, float spacing, int fontSize,
							Font const &font) {
		auto e = reg.spawn_entity();
		reg.emplace_component<component::position>(e, pos.x, pos.y);

		component::text t;
		t.content = content;
		t.spacing = spacing;
		t.font_size = fontSize;
		t.font = font;
		t.color = color;
		reg.add_component<component::text>(e, std::move(t));

		component::drawable drawable;
		reg.add_component<component::drawable>(e, std::move(drawable));

		reg.emplace_component<component::type>(e, component::entity_type::TEXT);
		return e;
	}
}
