/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** png
*/

#include "Include/png.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_png(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath, const std::string &modelPath)
    {
        auto png = reg.spawn_entity();
        reg.emplace_component<component::position>(png, x, y, z);
        reg.emplace_component<component::previous_position>(png, x, y, z);
        reg.emplace_component<component::velocity>(png, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(png, component::entity_type::IMAGE);
        reg.emplace_component<component::collision_box>(png, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = WHITE;
        reg.add_component<component::drawable>(png, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(png, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(png, std::move(model));
        }

        create_hitbox_for(reg, png);
        return png;
    }

}
