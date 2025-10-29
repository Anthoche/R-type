/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** obstacle
*/

#include "Include/obstacle.hpp"
#include "Include/hitbox.hpp"
#include "iostream"

namespace game::entities {

    ecs::entity_t create_obstacle(ecs::registry &reg, float x, float y, float z,
        const std::string &imagePath, const std::string &modelPath, float velocity, float width, float height) {
        auto obstacle = reg.spawn_entity();

        float _velocity = (velocity * 100.0f) * -1.0f;
        reg.emplace_component<component::position>(obstacle, x, y, z);
        reg.emplace_component<component::previous_position>(obstacle, x, y, z);
        reg.emplace_component<component::velocity>(obstacle, _velocity, 0.f, 0.f);

        reg.emplace_component<component::health>(obstacle, 9999, 9999);
        reg.emplace_component<component::type>(obstacle, component::entity_type::OBSTACLE);

        reg.emplace_component<component::collision_box>(obstacle, width, height, 50.f);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = 50.f;
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

        return obstacle;
    }
}
