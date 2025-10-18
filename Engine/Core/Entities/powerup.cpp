/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** powerup
*/

#include "Include/powerup.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_powerup(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath,  const std::string &modelPath) {
        auto powerup = reg.spawn_entity();
        reg.emplace_component<component::position>(powerup, x, y, z);
        reg.emplace_component<component::previous_position>(powerup, x, y, z);
        reg.emplace_component<component::velocity>(powerup, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(powerup, component::entity_type::POWERUP);
        reg.emplace_component<component::collision_box>(powerup, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = GREEN;
        reg.add_component<component::drawable>(powerup, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(powerup, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(powerup, std::move(model));
        }

        create_hitbox_for(reg, powerup);
        return powerup;
    }

}
