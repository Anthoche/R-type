/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** BackgroundTests.cpp
*/

#include <gtest/gtest.h>
#include "background.hpp"
#include "registry.hpp"
#include "components.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

static void register_all_background_components(registry &reg)
{
    reg.register_component<position>();
    reg.register_component<drawable>();
    reg.register_component<type>();
    reg.register_component<sprite>();
    reg.register_component<model3D>();
}

TEST(Background, creates_basic_components)
{
    registry reg;
    register_all_background_components(reg);

    auto bg = create_background(reg, 0.f, 0.f);
    auto &positions = reg.get_components<position>();
    auto &drawables = reg.get_components<drawable>();
    auto &types = reg.get_components<type>();

    EXPECT_TRUE(positions[static_cast<size_t>(bg)].has_value());
    EXPECT_TRUE(drawables[static_cast<size_t>(bg)].has_value());
    EXPECT_TRUE(types[static_cast<size_t>(bg)].has_value());
    EXPECT_EQ(types[static_cast<size_t>(bg)]->value, entity_type::BACKGROUND);
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(bg)]->width, 800.f);
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(bg)]->height, 600.f);
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(bg)]->depth, 1.f);
}

TEST(Background, sets_position_correctly)
{
    registry reg;
    register_all_background_components(reg);

    auto bg = create_background(reg, 10.f, 20.f, 30.f);
    auto &positions = reg.get_components<position>();

    ASSERT_TRUE(positions[static_cast<size_t>(bg)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(bg)]->x, 10.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(bg)]->y, 20.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(bg)]->z, 30.f);
}

TEST(Background, optional_sprite_model_added)
{
    registry reg;
    register_all_background_components(reg);

    auto bg = create_background(reg, 0.f, 0.f, 0.f, 800.f, 600.f, 1.f,
        "background.png", "skybox.obj", 2.f);
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();

    ASSERT_TRUE(sprites[static_cast<size_t>(bg)].has_value());
    ASSERT_TRUE(models[static_cast<size_t>(bg)].has_value());

    EXPECT_EQ(sprites[static_cast<size_t>(bg)]->image_path, "background.png");
    EXPECT_EQ(models[static_cast<size_t>(bg)]->model_path, "skybox.obj");
    EXPECT_FLOAT_EQ(sprites[static_cast<size_t>(bg)]->scale, 2.f);
    EXPECT_FLOAT_EQ(models[static_cast<size_t>(bg)]->scale, 2.f);
}

TEST(Background, empty_paths_skip_components)
{
    registry reg;
    register_all_background_components(reg);

    auto bg = create_background(reg, 0.f, 0.f, 0.f, 100.f, 100.f, 1.f, "", "");
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();

    EXPECT_FALSE(sprites[static_cast<size_t>(bg)].has_value());
    EXPECT_FALSE(models[static_cast<size_t>(bg)].has_value());
}

TEST(Background, multiple_entities_unique)
{
    registry reg;
    register_all_background_components(reg);

    auto b1 = create_background(reg, 0.f, 0.f);
    auto b2 = create_background(reg, 100.f, 100.f);
    EXPECT_NE(b1, b2);

    auto &positions = reg.get_components<position>();
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(b1)]->x, 0.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(b2)]->x, 100.f);
}

TEST(Background, scale_applied_to_sprite_model)
{
    registry reg;
    register_all_background_components(reg);

    auto bg = create_background(reg, 0.f, 0.f, 0.f, 800.f, 600.f, 1.f,
        "layer.png", "sky.obj", 3.5f);
    auto &sprites = reg.get_components<sprite>();
    auto &models = reg.get_components<model3D>();

    EXPECT_FLOAT_EQ(sprites[static_cast<size_t>(bg)]->scale, 3.5f);
    EXPECT_FLOAT_EQ(models[static_cast<size_t>(bg)]->scale, 3.5f);
}
