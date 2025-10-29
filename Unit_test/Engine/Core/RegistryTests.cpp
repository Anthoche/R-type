/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_registry.cpp
*/

#include <cstdint>
#include <gtest/gtest.h>
#include <unordered_set>
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

TEST(Registry, reuse_entity) {
    ecs::registry reg;
    ecs::entity_t e1 = reg.spawn_entity();
    ecs::entity_t e2 = reg.spawn_entity();

    EXPECT_NE(e1, e2);

    reg.kill_entity(e1);
    ecs::entity_t e3 = reg.spawn_entity();

    EXPECT_EQ((std::size_t)e1, (std::size_t)e3);
}

struct Position {
    int x;
    int y;
};

TEST(Registry, remove_component) {
    ecs::registry reg;

    reg.register_component<Position>();
    ecs::entity_t e = reg.spawn_entity();
    reg.add_component<Position>(e, {5, 15});
    reg.remove_component<Position>(e);
    auto &arr = reg.get_components<Position>();

    EXPECT_FALSE(arr[static_cast<std::size_t>(e)].has_value());
}

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

TEST(Registry, emplace_component) {
    ecs::registry reg;
    reg.register_component<Position>();
    ecs::entity_t e = reg.spawn_entity();
    auto &pos = reg.emplace_component<Position>(e, 42, 99);

    EXPECT_TRUE(pos.has_value());
    EXPECT_EQ(pos->x, 42);
    EXPECT_EQ(pos->y, 99);
}

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

TEST(Registry, spawn_many_entities) {
    ecs::registry reg;
    const int N = 1000;
    std::vector<ecs::entity_t> entities;

    for (int i = 0; i < N; ++i) {
        entities.push_back(reg.spawn_entity());
    }

    std::unordered_set<std::size_t> ids;
    for (auto &e : entities) ids.insert(e.value());
    EXPECT_EQ(ids.size(), N);
}

TEST(Registry, kill_multiple_and_reuse) {
    ecs::registry reg;
    auto e1 = reg.spawn_entity();
    auto e2 = reg.spawn_entity();
    auto e3 = reg.spawn_entity();
    reg.kill_entity(e2);
    reg.kill_entity(e1);
    auto e4 = reg.spawn_entity();
    auto e5 = reg.spawn_entity();

    EXPECT_TRUE((e4.value() == e1.value() || e4.value() == e2.value()));
    EXPECT_TRUE((e5.value() == e1.value() || e5.value() == e2.value()));
    EXPECT_NE(e4, e5);
}

struct Velocity { 
    int dx;
    int dy; 
};

TEST(Registry, system_updates_multiple_components) {
    ecs::registry reg;
    reg.register_component<Position>();
    reg.register_component<Velocity>();
    auto e = reg.spawn_entity();
    reg.add_component<Position>(e, {0, 0});
    reg.add_component<Velocity>(e, {1, 2});

    reg.add_system<Position, Velocity>([](ecs::registry &, ecs::sparse_array<Position> &pos, ecs::sparse_array<Velocity> &vel) {
        for (size_t i = 0; i < pos.size(); ++i) {
            if (pos[i].has_value() && vel[i].has_value()) {
                pos[i]->x += vel[i]->dx;
                pos[i]->y += vel[i]->dy;
            }
        }
    });

    reg.run_systems();
    auto &p = reg.get_components<Position>()[e.value()];
    EXPECT_EQ(p->x, 1);
    EXPECT_EQ(p->y, 2);
}

TEST(Registry, clear_registry_resets_all) {
    ecs::registry reg;
    reg.register_component<Position>();

    auto e1 = reg.spawn_entity();
    reg.add_component<Position>(e1, {5, 5});

    reg.clear();
    EXPECT_EQ(reg.alive_entities().size(), 0);
    EXPECT_THROW(reg.get_components<Position>(), std::runtime_error);
    auto e2 = reg.spawn_entity();
    EXPECT_EQ(e2.value(), 0);
}

TEST(Registry, multiple_systems_interact) {
    ecs::registry reg;
    reg.register_component<Position>();

    auto e = reg.spawn_entity();
    reg.add_component<Position>(e, {0, 0});
    reg.add_system<Position>([](ecs::registry &, ecs::sparse_array<Position> &pos){
        for (auto &p : pos) if (p.has_value()) p->x += 10;
    });
    reg.add_system<Position>([](ecs::registry &, ecs::sparse_array<Position> &pos){
        for (auto &p : pos) if (p.has_value()) p->y += 20;
    });

    reg.run_systems();
    auto &p = reg.get_components<Position>()[e.value()];
    EXPECT_EQ(p->x, 10);
    EXPECT_EQ(p->y, 20);
}

TEST(Registry, alive_entities_returns_correct_list) {
    ecs::registry reg;
    auto e1 = reg.spawn_entity();
    auto e2 = reg.spawn_entity();
    auto e3 = reg.spawn_entity();

    reg.kill_entity(e2);

    auto alive = reg.alive_entities();
    std::unordered_set<std::size_t> ids;
    for (auto &e : alive) ids.insert(e.value());

    EXPECT_TRUE(ids.find(e1.value()) != ids.end());
    EXPECT_TRUE(ids.find(e3.value()) != ids.end());
    EXPECT_TRUE(ids.find(e2.value()) == ids.end());
}
