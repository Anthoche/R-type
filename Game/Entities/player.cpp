/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** player
*/

#include "Include/player.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

ecs::entity_t create_player(ecs::registry &reg, float x, float y, const std::string &imagePath) {
	auto player = reg.spawn_entity();
	
	reg.emplace_component<component::position>(player, x, y);
	
	reg.emplace_component<component::velocity>(player, 0.f, 0.f);
	
	reg.emplace_component<component::controllable>(player, 300.f);
	
	reg.emplace_component<component::health>(player, 100, 100);
	
	reg.emplace_component<component::type>(player, component::entity_type::PLAYER);
	
	reg.emplace_component<component::collision_box>(player, 50.f, 30.f);
	
	component::drawable drawable;
	drawable.width = 50.f;
	drawable.height = 30.f;
	drawable.r = 0.f; drawable.g = 1.f; drawable.b = 1.f; drawable.a = 1.f;
	reg.add_component<component::drawable>(player, std::move(drawable));

	if (!imagePath.empty()) {
		component::sprite spr;
		spr.image_path = imagePath;
		spr.scale = 1.f;
		reg.add_component<component::sprite>(player, std::move(spr));
	}
	
	create_hitbox_for(reg, player);

	return player;
}

void setup_player_control_system(ecs::registry &reg) {
	reg.add_system<component::velocity, component::controllable>(
		[](ecs::registry &reg,
		   ecs::sparse_array<component::velocity> &vel,
		   ecs::sparse_array<component::controllable> &ctrl) {
			// TODO: Implémenter un système d'input générique
		});
}

void setup_player_bounds_system(ecs::registry &reg, float screen_width, float screen_height) {
	reg.add_system<component::position, component::type>(
		[screen_width, screen_height](ecs::registry &reg,
		   ecs::sparse_array<component::position> &pos,
		   ecs::sparse_array<component::type> &type) {
			for (std::size_t i = 0; i < pos.size() && i < type.size(); ++i) {
				if (pos[i] && type[i] && type[i]->value == component::entity_type::PLAYER) {
					if (pos[i]->x < 0) pos[i]->x = 0;
					if (pos[i]->x > screen_width - 50) pos[i]->x = screen_width - 50;
					
					if (pos[i]->y < 0) pos[i]->y = 0;
					if (pos[i]->y > screen_height - 30) pos[i]->y = screen_height - 30;
				}
			}
		});
}

}
