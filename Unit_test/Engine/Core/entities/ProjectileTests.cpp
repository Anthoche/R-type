/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ProjectileTests.cpp
*/

#include <gtest/gtest.h>
#include "registry.hpp"
#include "projectile.hpp"
#include "hitbox.hpp"
#include "components.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

TEST(Projectile, creates_entity_all_components) {
    registry reg;
    reg.register_component<position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<damage>();
    reg.register_component<drawable>();
    reg.register_component<collision_box>();
    reg.register_component<hitbox_link>();

    auto p = create_projectile(reg, 1.f, 2.f, 3.f, 10.f, 20.f, 30.f, 42);
    auto &positions = reg.get_components<position>();
    auto &velocities = reg.get_components<velocity>();
    auto &types = reg.get_components<type>();
    auto &damages = reg.get_components<damage>();
    auto &drawables = reg.get_components<drawable>();
    auto &hitboxes = reg.get_components<hitbox_link>();

    EXPECT_TRUE(positions[static_cast<size_t>(p)].has_value());
    EXPECT_TRUE(velocities[static_cast<size_t>(p)].has_value());
    EXPECT_TRUE(types[static_cast<size_t>(p)].has_value());
    EXPECT_TRUE(damages[static_cast<size_t>(p)].has_value());
    EXPECT_TRUE(drawables[static_cast<size_t>(p)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(p)]->x, 1.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(p)]->y, 2.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(p)]->z, 3.f);
    EXPECT_FLOAT_EQ(velocities[static_cast<size_t>(p)]->vx, 10.f);
    EXPECT_FLOAT_EQ(velocities[static_cast<size_t>(p)]->vy, 20.f);
    EXPECT_FLOAT_EQ(velocities[static_cast<size_t>(p)]->vz, 30.f);
    EXPECT_EQ(types[static_cast<size_t>(p)]->value, entity_type::PROJECTILE);
    EXPECT_EQ(damages[static_cast<size_t>(p)]->amount, 10);

    bool hitbox_found = false;
    for (size_t i = 0; i < hitboxes.size(); ++i) {
        if (hitboxes[i] && hitboxes[i]->owner == p) {
            hitbox_found = true;
            break;
        }
    }
    EXPECT_TRUE(hitbox_found);
}

TEST(Projectile, multiple_unique_entities) {
    registry reg;
    reg.register_component<position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<damage>();
    reg.register_component<drawable>();
    reg.register_component<collision_box>();
    reg.register_component<hitbox_link>();

    auto p1 = create_projectile(reg, 0.f, 0.f, 0.f, 1.f, 1.f, 1.f, 1);
    auto p2 = create_projectile(reg, 1.f, 1.f, 1.f, 2.f, 2.f, 2.f, 2);
    EXPECT_NE(p1, p2);
    auto &positions = reg.get_components<position>();
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(p1)]->x, 0.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(p2)]->x, 1.f);
}
