/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** obstacle
*/

#include "Include/obstacle.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

ecs::entity_t create_obstacle(ecs::registry &reg, float x, float y) {
    auto obstacle = reg.spawn_entity();

    reg.emplace_component<component::position>(obstacle, x, y);

    reg.emplace_component<component::velocity>(obstacle, 0.f, 0.f);

    reg.emplace_component<component::health>(obstacle, 9999, 9999);

    reg.emplace_component<component::type>(obstacle, component::entity_type::OBSTACLE);

    reg.emplace_component<component::collision_box>(obstacle, 48.f, 48.f);

    component::drawable drawable;
    drawable.width = 48.f;
    drawable.height = 48.f;
    drawable.r = 0.6f; drawable.g = 0.6f; drawable.b = 0.6f; drawable.a = 1.f;
    reg.add_component<component::drawable>(obstacle, std::move(drawable));

    create_hitbox_for(reg, obstacle);

    return obstacle;
}

}


