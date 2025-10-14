/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_registry.cpp
*/

#include <cstdint>
#include <gtest/gtest.h>
#include "registry.hpp"

TEST(Registry, spawn_kill_entity) {
    ecs::registry reg;

    ecs::entity_t e1 = reg.spawn_entity();
    ecs::entity_t e2 = reg.spawn_entity();
    EXPECT_NE(e1, e2);
    reg.kill_entity(e1);

    ecs::entity_t e3 = reg.spawn_entity();
    EXPECT_EQ((std::size_t)e1, (std::size_t)e3);
}

// -------------------------
// Spawn & Kill Entities
// -------------------------
TEST(Registry, reuse_entity) {
    ecs::registry reg;
    ecs::entity_t e1 = reg.spawn_entity();
    ecs::entity_t e2 = reg.spawn_entity();

    EXPECT_NE(e1, e2);

    reg.kill_entity(e1);
    ecs::entity_t e3 = reg.spawn_entity();

    EXPECT_EQ((std::size_t)e1, (std::size_t)e3);
}

// -------------------------
// Register and add component
// -------------------------
struct Position {
    int x;
    int y;
};


// -------------------------
// Remove component
// -------------------------
TEST(Registry, remove_component) {
    ecs::registry reg;

    reg.register_component<Position>();
    ecs::entity_t e = reg.spawn_entity();
    reg.add_component<Position>(e, {5, 15});
    reg.remove_component<Position>(e);
    auto &arr = reg.get_components<Position>();

    EXPECT_FALSE(arr[static_cast<std::size_t>(e)].has_value());
}

// -------------------------
// Register and add component
// -------------------------
TEST(Registry, add_component) {
    ecs::registry reg;

    reg.register_component<Position>();

    ecs::entity_t e = reg.spawn_entity();
    auto &pos = reg.add_component<Position>(e, {10, 20});

    EXPECT_TRUE(pos.has_value());
    EXPECT_EQ(pos->x, 10);
    EXPECT_EQ(pos->y, 20);

    auto &arr = reg.get_components<Position>();
    EXPECT_TRUE(arr[static_cast<std::size_t>(e)].has_value());
}

// -------------------------
// Emplace component
// -------------------------
TEST(Registry, emplace_component) {
    ecs::registry reg;
    reg.register_component<Position>();
    ecs::entity_t e = reg.spawn_entity();
    auto &pos = reg.emplace_component<Position>(e, 42, 99);

    EXPECT_TRUE(pos.has_value());
    EXPECT_EQ(pos->x, 42);
    EXPECT_EQ(pos->y, 99);
}

// -------------------------
// Add and run system
// -------------------------
TEST(Registry, run_systems) {
    ecs::registry reg;
    reg.register_component<Position>();
    ecs::entity_t e = reg.spawn_entity();
    reg.add_component<Position>(e, {1, 1});

    reg.add_system<Position>([](ecs::registry &, ecs::sparse_array<Position> &positions) {
        for (auto &opt : positions) {
            if (opt.has_value()) {
                opt->x += 10;
                opt->y += 20;
            }
        }
    });

    reg.run_systems();
    auto &arr = reg.get_components<Position>();
    auto &pos = arr[static_cast<std::size_t>(e)];

    EXPECT_TRUE(pos.has_value());
    EXPECT_EQ(pos->x, 11);
    EXPECT_EQ(pos->y, 21);
}
