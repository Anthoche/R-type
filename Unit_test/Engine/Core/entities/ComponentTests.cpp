/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ComponentTests.cpp
*/

#include <criterion/criterion.h>
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace component;

Test(Component, position_creation) {
    registry reg;
    reg.register_component<component::position>();

    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {100.5f, 200.3f});

    auto &position = reg.get_components<position>();
    auto &pos = position[static_cast<std::size_t>(test)];

    cr_assert(pos.has_value(), "Position component should exist");
    cr_assert_eq(pos->x, 100.5f, "X coordinate should be correct");
    cr_assert_eq(pos->y, 200.3f, "Y coordinate should be correct");
}

Test(Component, position_emplace) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.emplace_component<position>(test, 50.0f, 75.0f);
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    
    cr_assert(pos.has_value());
    cr_assert_eq(pos->x, 50.0f);
    cr_assert_eq(pos->y, 75.0f);
}

Test(Component, position_modification) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {0.0f, 0.0f});
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    pos->x = 150.0f;
    pos->y = 250.0f;
    
    cr_assert_eq(pos->x, 150.0f);
    cr_assert_eq(pos->y, 250.0f);
}

Test(Component, position_removal) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {100.0f, 200.0f});
    
    reg.remove_component<position>(test);
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    
    cr_assert(!pos.has_value(), "Position component should not exist after removal");
}

Test(Component, position_multiple_entities) {
    registry reg;
    reg.register_component<position>();
    
    entity_t anthony = reg.spawn_entity();
    entity_t martin = reg.spawn_entity();
    entity_t manuel = reg.spawn_entity();
    
    reg.add_component<position>(anthony, {10.0f, 20.0f});
    reg.add_component<position>(martin, {30.0f, 40.0f});
    reg.add_component<position>(manuel, {50.0f, 60.0f});
    
    auto &positions = reg.get_components<position>();
    
    cr_assert(positions[static_cast<std::size_t>(anthony)].has_value());
    cr_assert_eq(positions[static_cast<std::size_t>(anthony)]->x, 10.0f);
    cr_assert_eq(positions[static_cast<std::size_t>(anthony)]->y, 20.0f);
    
    cr_assert(positions[static_cast<std::size_t>(martin)].has_value());
    cr_assert_eq(positions[static_cast<std::size_t>(martin)]->x, 30.0f);
    cr_assert_eq(positions[static_cast<std::size_t>(martin)]->y, 40.0f);
    
    cr_assert(positions[static_cast<std::size_t>(manuel)].has_value());
    cr_assert_eq(positions[static_cast<std::size_t>(manuel)]->x, 50.0f);
    cr_assert_eq(positions[static_cast<std::size_t>(manuel)]->y, 60.0f);
}

Test(Component, position_default_values) {
    registry reg;
    reg.register_component<position>();
    
    entity_t test = reg.spawn_entity();
    reg.add_component<position>(test, {});
    
    auto &positions = reg.get_components<position>();
    auto &pos = positions[static_cast<std::size_t>(test)];
    
    cr_assert(pos.has_value());
    cr_assert_eq(pos->x, 0.0f, "Default X should be 0.0f");
    cr_assert_eq(pos->y, 0.0f, "Default Y should be 0.0f");
}