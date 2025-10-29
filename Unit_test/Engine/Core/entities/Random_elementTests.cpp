/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Random_elementTests.cpp
*/

#include <gtest/gtest.h>
#include "registry.hpp"
#include "random_element.hpp"
#include "components.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

TEST(RandomElement, creates_with_position_drawable_type) {
    registry reg;
    reg.register_component<position>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
    reg.register_component<type>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<collision_box>();
    reg.register_component<pattern_element>();

    auto e = create_random_element(reg, 1.f, 2.f, 3.f, "img.png", 10.f, 20.f, "pattern_a", 0.5f);
    auto &positions = reg.get_components<position>();
    auto &drawables = reg.get_components<drawable>();
    auto &types = reg.get_components<type>();
    auto &sprites = reg.get_components<sprite>();
    auto &velocities = reg.get_components<velocity>();
    auto &patterns = reg.get_components<pattern_element>();
    auto &collisions = reg.get_components<collision_box>();
    auto &previous = reg.get_components<previous_position>();

    EXPECT_TRUE(positions[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(drawables[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(types[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(sprites[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(velocities[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(patterns[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(collisions[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(previous[static_cast<size_t>(e)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(e)]->x, 1.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(e)]->y, 2.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(e)]->z, 3.f);
    EXPECT_FLOAT_EQ(previous[static_cast<size_t>(e)]->x, 1.f);
    EXPECT_FLOAT_EQ(previous[static_cast<size_t>(e)]->y, 2.f);
    EXPECT_FLOAT_EQ(previous[static_cast<size_t>(e)]->z, 3.f);
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(e)]->width, 10.f);
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(e)]->height, 20.f);
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(e)]->depth, 28.f);
    EXPECT_FLOAT_EQ(collisions[static_cast<size_t>(e)]->width, 10.f);
    EXPECT_FLOAT_EQ(collisions[static_cast<size_t>(e)]->height, 20.f);
    EXPECT_FLOAT_EQ(collisions[static_cast<size_t>(e)]->depth, 28.f);
    EXPECT_FLOAT_EQ(velocities[static_cast<size_t>(e)]->vx, -50.f);
    EXPECT_FLOAT_EQ(velocities[static_cast<size_t>(e)]->vy, 0.f);
    EXPECT_EQ(types[static_cast<size_t>(e)]->value, entity_type::RANDOM_ELEMENT);
    EXPECT_EQ(patterns[static_cast<size_t>(e)]->pattern_name, "pattern_a");
}

TEST(RandomElement, creates_without_sprite_or_audio) {
    registry reg;
    reg.register_component<position>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
    reg.register_component<type>();
    reg.register_component<pattern_element>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<collision_box>();

    auto e = create_random_element(reg, 0.f, 0.f, 0.f, "", 5.f, 5.f, "", 0.f);
    auto &sprites = reg.get_components<sprite>();

    EXPECT_FALSE(sprites[static_cast<size_t>(e)].has_value());
}

TEST(RandomElement, multiple_entities_are_unique) {
    registry reg;
    reg.register_component<position>();
    reg.register_component<drawable>();
    reg.register_component<type>();
    reg.register_component<sprite>();
    reg.register_component<pattern_element>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<collision_box>();

    auto e1 = create_random_element(reg, 0.f, 0.f, 0.f, "", 1.f, 1.f, "", 0.f);
    auto e2 = create_random_element(reg, 1.f, 1.f, 1.f, "", 2.f, 2.f, "", 0.f);
    EXPECT_NE(e1, e2);
    auto &positions = reg.get_components<position>();
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(e1)]->x, 0.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(e2)]->x, 1.f);
}
