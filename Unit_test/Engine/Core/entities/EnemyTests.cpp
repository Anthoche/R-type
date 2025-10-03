/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EnemyTest.cpp
*/

#include <criterion/criterion.h>
#include "enemy.hpp"
#include "components.hpp"
#include "registry.hpp"
#include "hitbox.hpp"

using namespace ecs;
using namespace game::entities;

Test(Enemy, has_position_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();

    entity_t e = create_enemy(reg, 150.0f, 250.0f, "");

    auto &positions = reg.get_components<component::position>();
    auto &pos = positions[static_cast<std::size_t>(e)];

    cr_assert(pos.has_value());
    cr_assert_eq(pos->x, 150.0f);
    cr_assert_eq(pos->y, 250.0f);
}

Test(Enemy, has_velocity_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();

    entity_t e = create_enemy(reg, 0.0f, 0.0f, "");

    auto &velocities = reg.get_components<component::velocity>();
    auto &vel = velocities[static_cast<std::size_t>(e)];

    cr_assert(vel.has_value());
    cr_assert_eq(vel->vx, -100.0f);
    cr_assert_eq(vel->vy, 0.0f);
}

Test(Enemy, has_health_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();

    entity_t e = create_enemy(reg, 0.0f, 0.0f, "");

    auto &healths = reg.get_components<component::health>();
    auto &hp = healths[static_cast<std::size_t>(e)];

    cr_assert(hp.has_value());
    cr_assert_eq(hp->current, 50);
    cr_assert_eq(hp->max, 50);
}

Test(Enemy, has_type_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();

    entity_t e = create_enemy(reg, 0.0f, 0.0f, "");

    auto &types = reg.get_components<component::type>();
    auto &type_comp = types[static_cast<std::size_t>(e)];

    cr_assert(type_comp.has_value());
    cr_assert_eq(type_comp->value, component::entity_type::ENEMY);
}

Test(Enemy, no_sprite_when_empty_path) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();

    entity_t e = create_enemy(reg, 0.0f, 0.0f, "");

    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(e)];

    cr_assert(!spr.has_value(), "enemy should not have sprite if no path is given");
}

Test(Enemy, has_sprite_when_path_given) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();

    entity_t e = create_enemy(reg, 0.0f, 0.0f, "assets/enemy.png");

    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(e)];

    cr_assert(spr.has_value());
    cr_assert_str_eq(spr->image_path.c_str(), "assets/enemy.png");
    cr_assert_eq(spr->scale, 1.0f);
}

Test(EnemyAI, clamp_velocity_x) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();

    setup_enemy_ai_system(reg);

    entity_t e = create_enemy(reg, 0.0f, 0.0f, "");

    auto &vels = reg.get_components<component::velocity>();
    vels[static_cast<std::size_t>(e)]->vx = -50.0f;

    reg.run_systems();

    cr_assert_eq(vels[static_cast<std::size_t>(e)]->vx, -100.0f,
                 "enemy AI should clamp vx to -100.f if greater");
}
