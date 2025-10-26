/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DecorationTests.cpp
*/

#include <gtest/gtest.h>
#include "decoration.hpp"
#include "registry.hpp"
#include "components.hpp"
#include "hitbox.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

TEST(Decoration, creates_entity_with_all_components) {
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

    float x = 5.f, y = 10.f, z = 2.f;
    float width = 32.f, height = 32.f, depth = 32.f;
    std::string imagePath = "deco.png";
    std::string modelPath = "deco.obj";
    auto deco = create_decoration(reg, x, y, z, width, height, depth, imagePath, modelPath);
    auto &positions = reg.get_components<position>();
    auto &types = reg.get_components<type>();
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();
    auto &hitboxes = reg.get_components<hitbox_link>();

    EXPECT_TRUE(positions[static_cast<std::size_t>(deco)].has_value());
    EXPECT_TRUE(types[static_cast<std::size_t>(deco)].has_value());
    EXPECT_TRUE(sprites[static_cast<std::size_t>(deco)].has_value());
    EXPECT_TRUE(models[static_cast<std::size_t>(deco)].has_value());

    bool hitbox_found = false;
    for (size_t i = 0; i < hitboxes.size(); ++i) {
        if (hitboxes[i] && hitboxes[i]->owner == deco) {
            hitbox_found = true;
            break;
        }
    }
    EXPECT_TRUE(hitbox_found);
    EXPECT_EQ(types[static_cast<std::size_t>(deco)]->value, entity_type::DECORATION);
    EXPECT_EQ(sprites[static_cast<std::size_t>(deco)]->image_path, "deco.png");
    EXPECT_EQ(models[static_cast<std::size_t>(deco)]->model_path, "deco.obj");
}

TEST(Decoration, creates_entity_without_image_or_model) {
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

    auto deco = create_decoration(reg, 0.f, 0.f, 0.f, 32.f, 32.f, 32.f, "", "");
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();
    auto &types = reg.get_components<type>();

    EXPECT_FALSE(sprites[deco.value()].has_value());
    EXPECT_FALSE(models[deco.value()].has_value());
    EXPECT_TRUE(types[deco.value()].has_value());
}

TEST(Decoration, multiple_decorations_have_unique_entities) {
    registry reg;

    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<hitbox_link>();

    auto deco1 = create_decoration(reg, 1.f, 1.f, 0.f, 32.f, 32.f, 32.f, "", "");
    auto deco2 = create_decoration(reg, 2.f, 2.f, 0.f, 32.f, 32.f, 32.f, "", "");
    EXPECT_NE(deco1.value(), deco2.value());
    auto &hitboxes = reg.get_components<hitbox_link>();

    bool hitbox1 = false, hitbox2 = false;
    for (size_t i = 0; i < hitboxes.size(); ++i) {
        if (hitboxes[i]) {
            if (hitboxes[i]->owner == deco1) hitbox1 = true;
            if (hitboxes[i]->owner == deco2) hitbox2 = true;
        }
    }
    EXPECT_TRUE(hitbox1);
    EXPECT_TRUE(hitbox2);
}

TEST(Decoration, hitbox_position_sync) {
    registry reg;

    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<hitbox_link>();

    auto deco = create_decoration(reg, 5.f, 10.f, 2.f, 32.f, 32.f, 32.f, "", "");
    setup_hitbox_sync_system(reg);
    reg.run_systems();
    auto &positions = reg.get_components<position>();
    auto &hitboxes = reg.get_components<hitbox_link>();

    for (size_t i = 0; i < hitboxes.size(); ++i) {
        if (hitboxes[i] && hitboxes[i]->owner == deco) {
            EXPECT_EQ(positions[i]->x, positions[deco.value()]->x);
            EXPECT_EQ(positions[i]->y, positions[deco.value()]->y);
            EXPECT_EQ(positions[i]->z, positions[deco.value()]->z);
        }
    }
}