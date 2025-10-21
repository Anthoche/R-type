/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** spawner
*/

#include "Include/spawner.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

ecs::entity_t create_spawner(ecs::registry &reg, float x, float y, float z, float width,
    float height, float depth, const std::string &imagePath, const std::string &modelPath)
{
    auto spawner = reg.spawn_entity();
    reg.emplace_component<component::position>(spawner, x, y, z);
    reg.emplace_component<component::previous_position>(spawner, x, y, z);
    reg.emplace_component<component::velocity>(spawner, 0.f, 0.f, 0.f);
    reg.emplace_component<component::type>(spawner, component::entity_type::SPAWNER);
    reg.emplace_component<component::collision_box>(spawner, width, height, depth);

    component::drawable draw;
    draw.width = width;
    draw.height = height;
    draw.depth = depth;
    draw.color = ORANGE;
    reg.add_component<component::drawable>(spawner, std::move(draw));

    if (!imagePath.empty()) {
        component::sprite spr;
        spr.image_path = imagePath;
        spr.scale = 1.f;
        reg.add_component<component::sprite>(spawner, std::move(spr));
    }

    if (!modelPath.empty()) {
        component::model3D model;
        model.model_path = modelPath;
        model.scale = 1.f;
        reg.add_component<component::model3D>(spawner, std::move(model));
    }

    create_hitbox_for(reg, spawner);
    return spawner;
}

}
