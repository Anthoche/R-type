/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** decoration
*/

#include "Include/decoration.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_decoration(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath, const std::string &modelPath)
    {
        auto deco = reg.spawn_entity();
        reg.emplace_component<component::position>(deco, x, y, z);
        reg.emplace_component<component::previous_position>(deco, x, y, z);
        reg.emplace_component<component::velocity>(deco, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(deco, component::entity_type::DECORATION);
        reg.emplace_component<component::collision_box>(deco, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = PURPLE;
        reg.add_component<component::drawable>(deco, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(deco, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(deco, std::move(model));
        }

        create_hitbox_for(reg, deco);
        return deco;
    }

}
