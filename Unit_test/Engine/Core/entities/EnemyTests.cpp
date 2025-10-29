/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** EnemyTests.cpp
*/

#include <gtest/gtest.h>
#include "enemy.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

static void register_all_enemy_components(registry &reg)
{
    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<health>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
}

TEST(Enemy, creates_basic_components)
{
    registry reg;
    register_all_enemy_components(reg);

    auto enemy = create_enemy(reg);
    auto &positions = reg.get_components<position>();
    auto &prevs = reg.get_components<previous_position>();
    auto &vels = reg.get_components<velocity>();
    auto &healths = reg.get_components<health>();
    auto &types = reg.get_components<type>();
    auto &collisions = reg.get_components<collision_box>();
    auto &draws = reg.get_components<drawable>();

    EXPECT_TRUE(positions[static_cast<size_t>(enemy)].has_value());
    EXPECT_TRUE(prevs[static_cast<size_t>(enemy)].has_value());
    EXPECT_TRUE(vels[static_cast<size_t>(enemy)].has_value());
    EXPECT_TRUE(healths[static_cast<size_t>(enemy)].has_value());
    EXPECT_TRUE(types[static_cast<size_t>(enemy)].has_value());
    EXPECT_TRUE(collisions[static_cast<size_t>(enemy)].has_value());
    EXPECT_TRUE(draws[static_cast<size_t>(enemy)].has_value());

    EXPECT_EQ(types[static_cast<size_t>(enemy)]->value, entity_type::ENEMY);
    EXPECT_EQ(healths[static_cast<size_t>(enemy)]->current, 50);
    EXPECT_EQ(healths[static_cast<size_t>(enemy)]->max, 50);
    EXPECT_FLOAT_EQ(collisions[static_cast<size_t>(enemy)]->width, 40.f);
    EXPECT_FLOAT_EQ(draws[static_cast<size_t>(enemy)]->width, 40.f);
}

TEST(Enemy, sets_position_correctly)
{
    registry reg;
    register_all_enemy_components(reg);

    auto enemy = create_enemy(reg, 123.f, 456.f, 789.f);
    auto &positions = reg.get_components<position>();

    ASSERT_TRUE(positions[static_cast<size_t>(enemy)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(enemy)]->x, 123.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(enemy)]->y, 456.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(enemy)]->z, 789.f);
}

TEST(Enemy, adds_sprite_image_provided)
{
    registry reg;
    register_all_enemy_components(reg);

    auto enemy = create_enemy(reg, 0.f, 0.f, 0.f, "enemy.png");
    auto &sprites = reg.get_components<sprite>();

    ASSERT_TRUE(sprites[static_cast<size_t>(enemy)].has_value());
    EXPECT_EQ(sprites[static_cast<size_t>(enemy)]->image_path, "enemy.png");
}

TEST(Enemy, skips_sprite_no_image)
{
    registry reg;
    register_all_enemy_components(reg);

    auto enemy = create_enemy(reg);
    auto &sprites = reg.get_components<sprite>();

    EXPECT_FALSE(sprites[static_cast<size_t>(enemy)].has_value());
}

TEST(Enemy, multiple_enemies_unique_positions)
{
    registry reg;
    register_all_enemy_components(reg);

    auto e1 = create_enemy(reg, 100.f, 100.f);
    auto e2 = create_enemy(reg, 200.f, 200.f);

    EXPECT_NE(e1, e2);

    auto &positions = reg.get_components<position>();
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(e1)]->x, 100.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(e2)]->x, 200.f);
}

TEST(Enemy, ai_system_sets_velocity_left)
{
    registry reg;
    register_all_enemy_components(reg);

    auto enemy = create_enemy(reg, 0.f, 0.f);
    setup_enemy_ai_system(reg);

    auto &vels = reg.get_components<velocity>();
    auto &types = reg.get_components<type>();
    vels[static_cast<size_t>(enemy)]->vx = -10.f;
    reg.run_systems();

    EXPECT_LE(vels[static_cast<size_t>(enemy)]->vx, -100.f);
}

TEST(Enemy, default_velocity_negative)
{
    registry reg;
    register_all_enemy_components(reg);

    auto enemy = create_enemy(reg);
    auto &vels = reg.get_components<velocity>();

    ASSERT_TRUE(vels[static_cast<size_t>(enemy)].has_value());
    EXPECT_LT(vels[static_cast<size_t>(enemy)]->vx, 0.f);
}
