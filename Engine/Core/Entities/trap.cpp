/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** trap
*/

#include "Include/trap.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_trap(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath, const std::string &modelPath)
    {
        auto trap = reg.spawn_entity();
        reg.emplace_component<component::position>(trap, x, y, z);
        reg.emplace_component<component::previous_position>(trap, x, y, z);
        reg.emplace_component<component::velocity>(trap, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(trap, component::entity_type::TRAP);
        reg.emplace_component<component::collision_box>(trap, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = RED;
        reg.add_component<component::drawable>(trap, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(trap, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(trap, std::move(model));
        }

        create_hitbox_for(reg, trap);
        return trap;
    }

}
