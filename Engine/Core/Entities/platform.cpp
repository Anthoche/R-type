/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** platform
*/


#include "Include/platform.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_platform(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath, const std::string &modelPath)
    {
        auto platform = reg.spawn_entity();
        reg.emplace_component<component::position>(platform, x, y, z);
        reg.emplace_component<component::previous_position>(platform, x, y, z);
        reg.emplace_component<component::velocity>(platform, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(platform, component::entity_type::PLATFORM);
        reg.emplace_component<component::collision_box>(platform, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = BROWN;
        reg.add_component<component::drawable>(platform, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(platform, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(platform, std::move(model));
        }

        create_hitbox_for(reg, platform);
        return platform;
    }

}
