/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** gate
*/

#include "Include/gate.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_gate(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath, const std::string &modelPath)
    {
        auto gate = reg.spawn_entity();
        reg.emplace_component<component::position>(gate, x, y, z);
        reg.emplace_component<component::previous_position>(gate, x, y, z);
        reg.emplace_component<component::velocity>(gate, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(gate, component::entity_type::GATE);
        reg.emplace_component<component::collision_box>(gate, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = BLUE;
        reg.add_component<component::drawable>(gate, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(gate, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(gate, std::move(model));
        }

        create_hitbox_for(reg, gate);
        return gate;
    }

}
