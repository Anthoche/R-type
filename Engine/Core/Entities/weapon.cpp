/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** weapon
*/

#include "Include/weapon.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_weapon(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath, const std::string &modelPath)
    {
        auto weapon = reg.spawn_entity();
        reg.emplace_component<component::position>(weapon, x, y, z);
        reg.emplace_component<component::previous_position>(weapon, x, y, z);
        reg.emplace_component<component::velocity>(weapon, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(weapon, component::entity_type::WEAPON);
        reg.emplace_component<component::collision_box>(weapon, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = ORANGE;
        reg.add_component<component::drawable>(weapon, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(weapon, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(weapon, std::move(model));
        }

        create_hitbox_for(reg, weapon);
        return weapon;
    }

}
