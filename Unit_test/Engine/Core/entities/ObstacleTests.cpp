/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ObstacleTests.cpp
*/

#include <gtest/gtest.h>
#include "registry.hpp"
#include "obstacle.hpp"
#include "hitbox.hpp"
#include "components.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

static void register_all_obstacle_components(registry &reg) {
    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<health>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
    reg.register_component<model3D>();
    reg.register_component<hitbox_link>();
}

TEST(Obstacle, creates_all_components) {
    registry reg;
    register_all_obstacle_components(reg);

    auto obs = create_obstacle(reg, 10.f, 20.f, 30.f, "img.png", "model.obj");
    auto &positions = reg.get_components<position>();
    auto &velocities = reg.get_components<velocity>();
    auto &healths = reg.get_components<health>();
    auto &types = reg.get_components<type>();
    auto &drawables = reg.get_components<drawable>();
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();
    auto &collisions = reg.get_components<collision_box>();

    EXPECT_TRUE(positions[static_cast<size_t>(obs)].has_value());
    EXPECT_TRUE(velocities[static_cast<size_t>(obs)].has_value());
    EXPECT_TRUE(healths[static_cast<size_t>(obs)].has_value());
    EXPECT_TRUE(types[static_cast<size_t>(obs)].has_value());
    EXPECT_TRUE(drawables[static_cast<size_t>(obs)].has_value());
    EXPECT_TRUE(sprites[static_cast<size_t>(obs)].has_value());
    EXPECT_TRUE(models[static_cast<size_t>(obs)].has_value());
    EXPECT_TRUE(collisions[static_cast<size_t>(obs)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(obs)]->x, 10.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(obs)]->y, 20.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(obs)]->z, 30.f);
    EXPECT_EQ(healths[static_cast<size_t>(obs)]->current, 9999);
    EXPECT_EQ(healths[static_cast<size_t>(obs)]->max, 9999);
    EXPECT_EQ(types[static_cast<size_t>(obs)]->value, entity_type::OBSTACLE);
    EXPECT_FLOAT_EQ(collisions[static_cast<size_t>(obs)]->width, 0.f);
    EXPECT_FLOAT_EQ(collisions[static_cast<size_t>(obs)]->height, 0.f);
}

TEST(Obstacle, optional_image_model) {
    registry reg;
    register_all_obstacle_components(reg);
    auto obs = create_obstacle(reg, 0.f, 0.f, 0.f);
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();

    EXPECT_FALSE(sprites[static_cast<size_t>(obs)].has_value());
    EXPECT_FALSE(models[static_cast<size_t>(obs)].has_value());
}

TEST(Obstacle, multiple_unique_entities) {
    registry reg;
    register_all_obstacle_components(reg);

    auto o1 = create_obstacle(reg, 1.f, 1.f, 1.f);
    auto o2 = create_obstacle(reg, 2.f, 2.f, 2.f);
    EXPECT_NE(o1, o2);
    auto &positions = reg.get_components<position>();
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(o1)]->x, 1.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(o2)]->x, 2.f);
}