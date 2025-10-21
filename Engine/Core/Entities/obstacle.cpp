/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** obstacle
*/

#include "Include/obstacle.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_obstacle(ecs::registry &reg, float x, float y, float z,
        const std::string &imagePath, const std::string &modelPath) {
        auto obstacle = reg.spawn_entity();

        // ====== Position / Movement ======
        reg.emplace_component<component::position>(obstacle, x, y, z);
        reg.emplace_component<component::previous_position>(obstacle, x, y, z);
        reg.emplace_component<component::velocity>(obstacle, 0.f, 0.f, 0.f);

        // ====== Stats ======
        reg.emplace_component<component::health>(obstacle, 9999, 9999);
        reg.emplace_component<component::type>(obstacle, component::entity_type::OBSTACLE);

        // ====== Collision ======
        reg.emplace_component<component::collision_box>(obstacle, 130.f, 50.f, 50.f);

        // ====== Visuals ======
        component::drawable draw;
        draw.width = 130.f;
        draw.height = 50.f;
        draw.depth = 50.f; // 3D depth
        draw.color = GRAY;
        reg.add_component<component::drawable>(obstacle, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(obstacle, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(obstacle, std::move(model));
        }

        // ====== Hitbox ======
        create_hitbox_for(reg, obstacle);

        return obstacle;
    }
}
