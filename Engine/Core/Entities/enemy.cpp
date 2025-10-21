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
        const std::string &imagePath, const std::string &modelPath, const std::string &pattern) {
        auto enemy = reg.spawn_entity();

        reg.emplace_component<component::position>(enemy, x, y, z);
        reg.emplace_component<component::previous_position>(enemy, x, y, z);
        reg.emplace_component<component::velocity>(enemy, -100.f, 0.f, 0.f);
        reg.emplace_component<component::velocity>(enemy, -30.f, 0.f);
        reg.emplace_component<component::health>(enemy, 50, 50);
        reg.emplace_component<component::type>(enemy, component::entity_type::ENEMY);
        reg.emplace_component<component::collision_box>(enemy, 40.f, 28.f, 28.f);
        reg.emplace_component<component::pattern_element>(enemy, pattern);

        component::drawable draw;
        draw.width = 40.f;
        draw.height = 28.f;
        draw.depth = 28.f;
        draw.color = RED;
        reg.add_component<component::drawable>(enemy, std::move(draw));

    if (!imagePath.empty()) {
        std::cout << "[DEBUG] Creating enemy with sprite: " << imagePath << std::endl;
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
                        // Simple AI: always move left
                        if (vel[i]->vx > -100.f) vel[i]->vx = -100.f;
                    }
                }
            });
    }
}
