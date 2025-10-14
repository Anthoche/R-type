/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ComponentTests.cpp
*/

#include <gtest/gtest.h>
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace component;

TEST(Component, position_creation) {
    registry reg;
    reg.register_component<component::position>();

    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {100.5f, 200.3f});

    auto &positions = reg.get_components<component::position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    ASSERT_TRUE(pos.has_value()) << "Position component should exist";
    EXPECT_FLOAT_EQ(pos->x, 100.5f);
    EXPECT_FLOAT_EQ(pos->y, 200.3f);
}

TEST(Component, position_emplace) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.emplace_component<position>(test, 50.0f, 75.0f);
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    
    ASSERT_TRUE(pos.has_value());
    EXPECT_FLOAT_EQ(pos->x, 50.0f);
    EXPECT_FLOAT_EQ(pos->y, 75.0f);
}

TEST(Component, position_modification) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {0.0f, 0.0f});
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    pos->x = 150.0f;
    pos->y = 250.0f;
    
    EXPECT_FLOAT_EQ(pos->x, 150.0f);
    EXPECT_FLOAT_EQ(pos->y, 250.0f);
}

TEST(Component, position_removal) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {100.0f, 200.0f});
    
    reg.remove_component<position>(test);
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    
    EXPECT_FALSE(pos.has_value()) <<  "Position component should not exist after removal";
}

TEST(Component, position_multiple_entities) {
    registry reg;
    reg.register_component<position>();
    
    entity_t anthony = reg.spawn_entity();
    entity_t martin = reg.spawn_entity();
    entity_t manuel = reg.spawn_entity();
    
    reg.add_component<position>(anthony, {10.0f, 20.0f});
    reg.add_component<position>(martin, {30.0f, 40.0f});
    reg.add_component<position>(manuel, {50.0f, 60.0f});
    
    auto &positions = reg.get_components<position>();
    
    ASSERT_TRUE(positions[static_cast<std::size_t>(anthony)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(anthony)]->x, 10.0f);
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(anthony)]->y, 20.0f);
    
    ASSERT_TRUE(positions[static_cast<std::size_t>(martin)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(martin)]->x, 30.0f);
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(martin)]->y, 40.0f);
    
    ASSERT_TRUE(positions[static_cast<std::size_t>(manuel)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(manuel)]->x, 50.0f);
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(manuel)]->y, 60.0f);
}

TEST(Component, position_default_values) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {});
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    
    ASSERT_TRUE(pos.has_value());
    EXPECT_FLOAT_EQ(pos->x, 0.0f);
    EXPECT_FLOAT_EQ(pos->y, 0.0f);
}