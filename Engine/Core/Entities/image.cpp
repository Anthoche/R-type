/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** image
*/

#include "Include/image.hpp"

namespace game::entities {
	ecs::entity_t create_image(ecs::registry &reg, Texture2D texture, Vector2 pos, Vector2 size) {
		auto e = reg.spawn_entity();
		reg.emplace_component<component::position>(e, pos.x, pos.y);

		component::drawable drawable;
		drawable.width = size.x;
		drawable.height = size.y;
		reg.add_component<component::drawable>(e, std::move(drawable));

		component::sprite sprite;
		sprite.texture = texture;
		reg.add_component<component::sprite>(e, std::move(sprite));

		reg.emplace_component<component::type>(e, component::entity_type::IMAGE);
		return e;
	}

	ecs::entity_t create_image(ecs::registry &reg, Texture2D texture, DynamicPosition pos, Vector2 offset) {
		auto e = reg.spawn_entity();
		reg.emplace_component<component::dynamic_position>(e, pos, offset.x, offset.y);

		component::drawable drawable;
		drawable.width = texture.width;
		drawable.height = texture.height;
		reg.add_component<component::drawable>(e, std::move(drawable));

		component::sprite sprite;
		sprite.texture = texture;
		reg.add_component<component::sprite>(e, std::move(sprite));

		reg.emplace_component<component::type>(e, component::entity_type::IMAGE);
		return e;
	}
}
