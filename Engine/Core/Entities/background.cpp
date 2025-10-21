/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** background entity impl
*/

#include "Include/background.hpp"

namespace game::entities {

    ecs::entity_t create_background(ecs::registry &reg, float x, float y, float z, float width, float height,
        float depth, const std::string &imagePath, const std::string &modelPath, float scale) {
        auto e = reg.spawn_entity();

        // ====== Position ======
        reg.emplace_component<component::position>(e, x, y, z);

        // ====== Drawable ======
        component::drawable drw;
        drw.width = width;
        drw.height = height;
        drw.depth = depth;
        drw.color = WHITE;
        reg.add_component<component::drawable>(e, std::move(drw));

        // ====== Type ======
        reg.emplace_component<component::type>(e, component::entity_type::BACKGROUND);

        // ====== Optional Sprite ======
        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = scale;
            reg.add_component<component::sprite>(e, std::move(spr));
        }

        // ====== Optional 3D Model ======
        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = scale;
            reg.add_component<component::model3D>(e, std::move(model));
        }

        return e;
    }
}
