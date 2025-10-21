/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** checkpoint
*/

#include "Include/checkpoint.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

ecs::entity_t create_checkpoint(ecs::registry &reg, float x, float y, float z, float width,
    float height, float depth, const std::string &imagePath, const std::string &modelPath)
{
    auto cp = reg.spawn_entity();
    reg.emplace_component<component::position>(cp, x, y, z);
    reg.emplace_component<component::previous_position>(cp, x, y, z);
    reg.emplace_component<component::velocity>(cp, 0.f, 0.f, 0.f);
    reg.emplace_component<component::type>(cp, component::entity_type::CHECKPOINT);
    reg.emplace_component<component::collision_box>(cp, width, height, depth);

    component::drawable draw;
    draw.width = width;
    draw.height = height;
    draw.depth = depth;
    draw.color = WHITE;
    reg.add_component<component::drawable>(cp, std::move(draw));

    if (!imagePath.empty()) {
        component::sprite spr;
        spr.image_path = imagePath;
        spr.scale = 1.f;
        reg.add_component<component::sprite>(cp, std::move(spr));
    }

    if (!modelPath.empty()) {
        component::model3D model;
        model.model_path = modelPath;
        model.scale = 1.f;
        reg.add_component<component::model3D>(cp, std::move(model));
    }

    create_hitbox_for(reg, cp);
    return cp;
}

}
