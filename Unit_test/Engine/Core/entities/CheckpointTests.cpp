/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** test_checkpoint.cpp
*/

#include <gtest/gtest.h>
#include "registry.hpp"
#include "checkpoint.hpp"
#include "hitbox.hpp"
#include "components.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

TEST(Checkpoint, optional_parameters) {
    registry reg;

    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
    reg.register_component<model3D>();
    reg.register_component<hitbox_link>();

    float x = 0.f, y = 0.f, z = 0.f;
    float w = 10.f, h = 20.f, d = 30.f;
    auto e = create_checkpoint(reg, x, y, z, w, h, d, "", "");
    auto &sprites = reg.get_components<sprite>();
    EXPECT_FALSE(sprites[static_cast<std::size_t>(e)].has_value());
    auto &models = reg.get_components<model3D>();
    EXPECT_FALSE(models[static_cast<std::size_t>(e)].has_value());
}

TEST(Checkpoint, multiple_checkpoints_unique_entities) {
    registry reg;

    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
    reg.register_component<model3D>();
    reg.register_component<hitbox_link>();

    auto e1 = create_checkpoint(reg, 0,0,0,1,1,1,"img1","model1");
    auto e2 = create_checkpoint(reg, 1,1,1,2,2,2,"img2","model2");
    EXPECT_NE(e1, e2);
    auto &positions = reg.get_components<position>();
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(e1)]->x, 0.f);
    EXPECT_FLOAT_EQ(positions[static_cast<std::size_t>(e2)]->x, 1.f);
}

TEST(Checkpoint, creates_entity_with_all_components) {
    registry reg;

    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
    reg.register_component<model3D>();
    reg.register_component<hitbox_link>();

    auto e = create_checkpoint(reg, 5.f, 10.f, 2.f, 32.f, 64.f, 16.f, "cp.png", "cp.obj");
    auto &types = reg.get_components<type>();
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();
    auto &hitboxes = reg.get_components<hitbox_link>();

    EXPECT_TRUE(types[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(sprites[static_cast<size_t>(e)].has_value());
    EXPECT_TRUE(models[static_cast<size_t>(e)].has_value());

    bool hitbox_found = false;
    for (size_t i = 0; i < hitboxes.size(); ++i) {
        if (hitboxes[i] && hitboxes[i]->owner == e) {
            hitbox_found = true;
            break;
        }
    }
    EXPECT_TRUE(hitbox_found);
}

TEST(Checkpoint, hitbox_position_sync) {
    registry reg;

    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<hitbox_link>();

    auto e = create_checkpoint(reg, 3.f, 6.f, 9.f, 32.f, 32.f, 32.f, "", "");
    setup_hitbox_sync_system(reg);
    reg.run_systems();
    auto &positions = reg.get_components<position>();
    auto &hitboxes = reg.get_components<hitbox_link>();

    for (size_t i = 0; i < hitboxes.size(); ++i) {
        if (hitboxes[i] && hitboxes[i]->owner == e) {
            EXPECT_FLOAT_EQ(positions[i]->x, positions[static_cast<size_t>(e)]->x);
            EXPECT_FLOAT_EQ(positions[i]->y, positions[static_cast<size_t>(e)]->y);
            EXPECT_FLOAT_EQ(positions[i]->z, positions[static_cast<size_t>(e)]->z);
        }
    }
}