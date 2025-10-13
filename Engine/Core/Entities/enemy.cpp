/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** enemy
*/

#include "Include/enemy.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

ecs::entity_t create_enemy(ecs::registry &reg, float x, float y, const std::string &imagePath) {
    auto enemy = reg.spawn_entity();

    reg.emplace_component<component::position>(enemy, x, y);

    reg.emplace_component<component::velocity>(enemy, -100.f, 0.f);

    reg.emplace_component<component::health>(enemy, 50, 50);

    reg.emplace_component<component::type>(enemy, component::entity_type::ENEMY);

    reg.emplace_component<component::collision_box>(enemy, 40.f, 28.f);

    component::drawable drawable;
    drawable.width = 40.f;
    drawable.height = 28.f;
    drawable.color = RED;
    reg.add_component<component::drawable>(enemy, std::move(drawable));

    // Attach hitbox
    create_hitbox_for(reg, enemy);

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
           ecs::sparse_array<component::type> &type) {
            (void)reg;
            for (std::size_t i = 0; i < vel.size() && i < type.size(); ++i) {
                if (vel[i] && type[i] && type[i]->value == component::entity_type::ENEMY) {
                    if (vel[i]->vx > -100.f) vel[i]->vx = -100.f;
                }
            }
        });
}

}


