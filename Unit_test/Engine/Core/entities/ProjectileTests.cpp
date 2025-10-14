/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ProjectileTest.cpp
*/

#include <gtest/gtest.h>
#include "projectile.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_projectile_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::type>();
    reg.register_component<component::damage>();
    reg.register_component<component::drawable>();
    reg.register_component<component::collision_box>();
}

TEST(Projectile, has_position_and_velocity) {
    registry reg;
    register_all_projectile_components(reg);

    entity_t proj = create_projectile(reg, 50.f, 100.f, 5.f, -3.f, 1);

    auto &positions = reg.get_components<component::position>();
    auto &vels = reg.get_components<component::velocity>();

    EXPECT_TRUE(positions[static_cast<std::size_t>(proj)].has_value());
    EXPECT_EQ(positions[static_cast<std::size_t>(proj)]->x, 50.f);
    EXPECT_EQ(positions[static_cast<std::size_t>(proj)]->y, 100.f);

    EXPECT_TRUE(vels[static_cast<std::size_t>(proj)].has_value());
    EXPECT_EQ(vels[static_cast<std::size_t>(proj)]->vx, 5.f);
    EXPECT_EQ(vels[static_cast<std::size_t>(proj)]->vy, -3.f);
}

TEST(Projectile, has_type_projectile) {
    registry reg;
    register_all_projectile_components(reg);

    entity_t proj = create_projectile(reg, 0.f, 0.f, 0.f, 0.f, 1);

    auto &types = reg.get_components<component::type>();
    EXPECT_TRUE(types[static_cast<std::size_t>(proj)].has_value());
    EXPECT_EQ(types[static_cast<std::size_t>(proj)]->value, component::entity_type::PROJECTILE);
}

TEST(Projectile, has_collision_box) {
    registry reg;
    register_all_projectile_components(reg);

    entity_t proj = create_projectile(reg, 0.f, 0.f, 0.f, 0.f, 1);

    auto &boxes = reg.get_components<component::collision_box>();
    EXPECT_TRUE(boxes[static_cast<std::size_t>(proj)].has_value());
    EXPECT_EQ(boxes[static_cast<std::size_t>(proj)]->width, 10.f);
    EXPECT_EQ(boxes[static_cast<std::size_t>(proj)]->height, 5.f);
}
