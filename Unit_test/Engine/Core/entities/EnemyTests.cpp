/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EnemyTest.cpp
*/

#include <gtest/gtest.h>
#include "enemy.hpp"
#include "components.hpp"
#include "registry.hpp"
#include "hitbox.hpp"

using namespace ecs;
using namespace game::entities;

TEST(Enemy, has_position_component) {
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

    EXPECT_TRUE(pos.has_value());
    EXPECT_EQ(pos->x, 150.0f);
    EXPECT_EQ(pos->y, 250.0f);
}

TEST(Enemy, has_velocity_component) {
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

    EXPECT_TRUE(vel.has_value());
    EXPECT_EQ(vel->vx, -100.0f);
    EXPECT_EQ(vel->vy, 0.0f);
}

TEST(Enemy, has_health_component) {
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

    EXPECT_TRUE(hp.has_value());
    EXPECT_EQ(hp->current, 50);
    EXPECT_EQ(hp->max, 50);
}

TEST(Enemy, has_type_component) {
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

    EXPECT_TRUE(type_comp.has_value());
    EXPECT_EQ(type_comp->value, component::entity_type::ENEMY);
}

TEST(Enemy, no_sprite_when_empty_path) {
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

    EXPECT_FALSE(spr.has_value());
}

TEST(Enemy, has_sprite_when_path_given) {
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

    EXPECT_TRUE(spr.has_value());
    EXPECT_STREQ(spr->image_path.c_str(), "assets/enemy.png");
    EXPECT_EQ(spr->scale, 1.0f);
}

TEST(EnemyAI, clamp_velocity_x) {
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

    EXPECT_EQ(vels[static_cast<std::size_t>(e)]->vx, -100.0f);
}
