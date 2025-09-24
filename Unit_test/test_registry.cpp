/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_registry.cpp
*/

#include <criterion/criterion.h>
#include "registry.hpp"

Test(Registry, spawn_and_kill_entity) {
    ecs::registry reg;

    ecs::entity_t e1 = reg.spawn_entity();
    ecs::entity_t e2 = reg.spawn_entity();
    cr_assert_neq(e1, e2, "id must be unique");

    reg.kill_entity(e1);

    ecs::entity_t e3 = reg.spawn_entity();
    cr_assert_eq((std::size_t)e1, (
        std::size_t)e3,
        "killed id should be reused when spawning new entity");
}

// -------------------------
// Spawn & Kill Entities
// -------------------------
Test(Registry, spawn_and_reuse_entity) {
    ecs::registry reg;

    ecs::entity_t e1 = reg.spawn_entity();
    ecs::entity_t e2 = reg.spawn_entity();

    cr_assert_neq(e1, e2, "two spawned entities must have different IDs");

    reg.kill_entity(e1);
    ecs::entity_t e3 = reg.spawn_entity();

    cr_assert_eq((std::size_t)e1, (std::size_t)e3,
        "killed entity ID should be reused when spawning a new one");
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
Test(Registry, remove_component) {
    ecs::registry reg;

    reg.register_component<Position>();
    ecs::entity_t e = reg.spawn_entity();
    reg.add_component<Position>(e, {5, 15});

    reg.remove_component<Position>(e);
    auto &arr = reg.get_components<Position>();

    cr_assert(!arr[static_cast<std::size_t>(e)].has_value(),
        "entity should not have a Position component after removal");
}
// Register and add component
Test(Registry, register_and_add_component) {
    ecs::registry reg;

    reg.register_component<Position>();

    ecs::entity_t e = reg.spawn_entity();
    auto &pos = reg.add_component<Position>(e, {10, 20});

    cr_assert(pos.has_value(), "position component should exist after adding");
    cr_assert_eq(pos->x, 10, "position.x should be initialized correctly");
    cr_assert_eq(pos->y, 20, "position.y should be initialized correctly");

    auto &arr = reg.get_components<Position>();
    cr_assert(arr[static_cast<std::size_t>(e)].has_value(),
        "entity should have a Position component after adding");
}

// Emplace component
Test(Registry, emplace_component) {
    ecs::registry reg;

    reg.register_component<Position>();
    ecs::entity_t e = reg.spawn_entity();

    auto &pos = reg.emplace_component<Position>(e, 42, 99);

    cr_assert(pos.has_value(), "position component should exist after emplace");
    cr_assert_eq(pos->x, 42);
    cr_assert_eq(pos->y, 99);
}


// -------------------------
// Add and run system
// -------------------------
Test(Registry, run_systems) {
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

    cr_assert(pos.has_value());
    cr_assert_eq(pos->x, 11);
    cr_assert_eq(pos->y, 21);
}
