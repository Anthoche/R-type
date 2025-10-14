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
