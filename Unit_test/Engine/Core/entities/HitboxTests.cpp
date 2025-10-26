/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** HitboxTest.cpp
*/

#include <gtest/gtest.h>
#include "hitbox.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_hitbox_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::hitbox_link>();
}

TEST(Hitbox, sync_position_with_owner) {
    registry reg;
    register_all_hitbox_components(reg);
    setup_hitbox_sync_system(reg);

    entity_t owner = reg.spawn_entity();
    reg.emplace_component<component::position>(owner, 5.f, 5.f);
    reg.emplace_component<component::collision_box>(owner, 20.f, 20.f);

    entity_t hit = create_hitbox_for(reg, owner);

    auto &positions = reg.get_components<component::position>();

    positions[static_cast<std::size_t>(owner)]->x = 50.f;
    positions[static_cast<std::size_t>(owner)]->y = 60.f;

    reg.run_systems();

    auto &hitPos = positions[static_cast<std::size_t>(hit)];
    EXPECT_TRUE(hitPos.has_value());
    EXPECT_EQ(hitPos->x, 50.f);
    EXPECT_EQ(hitPos->y, 60.f);
}

TEST(Hitbox, sync_size_with_owner_drawable) {
    registry reg;
    register_all_hitbox_components(reg);
    setup_hitbox_sync_system(reg);

    entity_t owner = reg.spawn_entity();
    reg.emplace_component<component::drawable>(owner, 30.f, 40.f, 50.f);
    entity_t hit = create_hitbox_for(reg, owner);

    auto &boxes = reg.get_components<component::collision_box>();
    EXPECT_TRUE(boxes[static_cast<size_t>(hit)].has_value());
    EXPECT_FLOAT_EQ(boxes[static_cast<size_t>(hit)]->width, 30.f);
    EXPECT_FLOAT_EQ(boxes[static_cast<size_t>(hit)]->height, 40.f);
    EXPECT_FLOAT_EQ(boxes[static_cast<size_t>(hit)]->depth, 50.f);
}

TEST(Hitbox, multiple_hitboxes_independent) {
    registry reg;
    register_all_hitbox_components(reg);
    setup_hitbox_sync_system(reg);

    entity_t owner1 = reg.spawn_entity();
    entity_t owner2 = reg.spawn_entity();
    reg.emplace_component<component::position>(owner1, 10.f, 20.f, 30.f);
    reg.emplace_component<component::position>(owner2, 40.f, 50.f, 60.f);
    entity_t hit1 = create_hitbox_for(reg, owner1);
    entity_t hit2 = create_hitbox_for(reg, owner2);

    reg.run_systems();
    auto &positions = reg.get_components<component::position>();
    EXPECT_EQ(positions[static_cast<size_t>(hit1)]->x, 10.f);
    EXPECT_EQ(positions[static_cast<size_t>(hit1)]->y, 20.f);
    EXPECT_EQ(positions[static_cast<size_t>(hit1)]->z, 30.f);
    EXPECT_EQ(positions[static_cast<size_t>(hit2)]->x, 40.f);
    EXPECT_EQ(positions[static_cast<size_t>(hit2)]->y, 50.f);
    EXPECT_EQ(positions[static_cast<size_t>(hit2)]->z, 60.f);
}

TEST(Hitbox, created_without_owner_at_origin) {
    registry reg;
    register_all_hitbox_components(reg);
    setup_hitbox_sync_system(reg);

    entity_t hit = reg.spawn_entity();
    reg.emplace_component<component::hitbox_link>(hit, ecs::entity_t{999}, 0.f, 0.f, 0.f);
    reg.emplace_component<component::position>(hit, 0.f, 0.f, 0.f);
    reg.run_systems();
    
    auto &positions = reg.get_components<component::position>();
    EXPECT_EQ(positions[static_cast<size_t>(hit)]->x, 0.f);
    EXPECT_EQ(positions[static_cast<size_t>(hit)]->y, 0.f);
    EXPECT_EQ(positions[static_cast<size_t>(hit)]->z, 0.f);
}
