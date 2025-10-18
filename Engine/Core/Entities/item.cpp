/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** item
*/

#include "Include/item.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_item(ecs::registry &reg, float x, float y, float z, float width,
        float height, float depth, const std::string &imagePath,  const std::string &modelPath) {
        auto item = reg.spawn_entity();
        reg.emplace_component<component::position>(item, x, y, z);
        reg.emplace_component<component::previous_position>(item, x, y, z);
        reg.emplace_component<component::velocity>(item, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(item, component::entity_type::ITEM);
        reg.emplace_component<component::collision_box>(item, width, height, depth);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        draw.color = YELLOW;
        reg.add_component<component::drawable>(item, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(item, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(item, std::move(model));
        }

        create_hitbox_for(reg, item);
        return item;
    }

}
