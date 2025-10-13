/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** obstacle
*/

#include "Include/obstacle.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

ecs::entity_t create_obstacle(ecs::registry &reg, float x, float y, const std::string &imagePath) {
    auto obstacle = reg.spawn_entity();

    reg.emplace_component<component::position>(obstacle, x, y);
    
    reg.emplace_component<component::previous_position>(obstacle, x, y);

    reg.emplace_component<component::velocity>(obstacle, 0.f, 0.f);

    reg.emplace_component<component::health>(obstacle, 9999, 9999);

    reg.emplace_component<component::type>(obstacle, component::entity_type::OBSTACLE);

    reg.emplace_component<component::collision_box>(obstacle, 60.f, 60.f);

    component::drawable drawable;
    drawable.width = 60.f;
    drawable.height = 60.f;
    drawable.color = GRAY;
    reg.add_component<component::drawable>(obstacle, std::move(drawable));

    create_hitbox_for(reg, obstacle);

    if (!imagePath.empty()) {
        component::sprite spr;
        spr.image_path = imagePath;
        spr.scale = 1.f;
        reg.add_component<component::sprite>(obstacle, std::move(spr));
    }

    return obstacle;
}

}


