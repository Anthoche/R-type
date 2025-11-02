/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** enemy
*/

#include "Include/enemy.hpp"
#include "Include/hitbox.hpp"
#include <iostream>

namespace game::entities {

    ecs::entity_t create_enemy(ecs::registry &reg, float x, float y, float z,
        const std::string &imagePath, float width, float height, const std::string &modelPath, const std::string &pattern,
        int healthValue, float velocity) {
        auto enemy = reg.spawn_entity();

        float _velocity = (velocity * 100.0f) * -1.0f;
        reg.emplace_component<component::position>(enemy, x, y, z);
        reg.emplace_component<component::previous_position>(enemy, x, y, z);
        reg.emplace_component<component::velocity>(enemy, _velocity, 0.f);
        reg.emplace_component<component::health>(enemy, healthValue, healthValue);
        reg.emplace_component<component::type>(enemy, component::entity_type::ENEMY);
        reg.emplace_component<component::collision_box>(enemy, width, height, 28.f);
        reg.emplace_component<component::pattern_element>(enemy, pattern);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = 28.f;
        draw.color = RED;
        reg.add_component<component::drawable>(enemy, std::move(draw));

    if (!imagePath.empty()) {
        component::sprite spr;
        spr.image_path = imagePath;
        spr.scale = 1.f;
        reg.add_component<component::sprite>(enemy, std::move(spr));
    }

        return enemy;
    }

    void setup_enemy_ai_system(ecs::registry &reg) {
        reg.add_system<component::velocity, component::type>(
            [](ecs::registry &reg,
            ecs::sparse_array<component::velocity> &vel,
            ecs::sparse_array<component::type> &type) 
            {
                for (std::size_t i = 0; i < vel.size() && i < type.size(); ++i) {
                    if (vel[i] && type[i] && type[i]->value == component::entity_type::ENEMY) {
                        if (vel[i]->vx > -100.f) vel[i]->vx = -100.f;
                    }
                }
            });
    }
}
