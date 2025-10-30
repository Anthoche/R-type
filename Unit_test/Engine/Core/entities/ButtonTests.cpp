/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** ButtonTests.cpp
*/

#include <gtest/gtest.h>
#include "button.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

static void register_all_button_components(registry &reg)
{
    reg.register_component<position>();
    reg.register_component<clickable>();
    reg.register_component<hoverable>();
    reg.register_component<type>();
    reg.register_component<drawable>();
    reg.register_component<text>();
}

TEST(Button, creates_entity_with_all_components)
{
    registry reg;
    register_all_button_components(reg);

    auto btn = create_button(reg, "btn_play", "Play", 
                        100.f, 200.f, 0.f, 250.f, 60.f, WHITE, BLACK, 40);
    auto &positions = reg.get_components<position>();
    auto &clickables = reg.get_components<clickable>();
    auto &hoverables = reg.get_components<hoverable>();
    auto &types = reg.get_components<type>();
    auto &drawables = reg.get_components<drawable>();
    auto &texts = reg.get_components<text>();

    EXPECT_TRUE(positions[static_cast<size_t>(btn)].has_value());
    EXPECT_TRUE(clickables[static_cast<size_t>(btn)].has_value());
    EXPECT_TRUE(hoverables[static_cast<size_t>(btn)].has_value());
    EXPECT_TRUE(types[static_cast<size_t>(btn)].has_value());
    EXPECT_TRUE(drawables[static_cast<size_t>(btn)].has_value());
    EXPECT_TRUE(texts[static_cast<size_t>(btn)].has_value());
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(btn)]->x, 100.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(btn)]->y, 200.f);
    EXPECT_EQ(types[static_cast<size_t>(btn)]->value, entity_type::BUTTON);
    EXPECT_EQ(clickables[static_cast<size_t>(btn)]->id, "btn_play");
    EXPECT_EQ(hoverables[static_cast<size_t>(btn)]->id, "btn_play");
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->content, "Play");
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->font_size, 40);
}

TEST(Button, creates_button_with_defaults)
{
    registry reg;
    register_all_button_components(reg);

    auto btn = create_button(reg, "btn_exit", "Exit", 0.f, 0.f);
    auto &positions = reg.get_components<position>();
    auto &drawables = reg.get_components<drawable>();
    auto &texts = reg.get_components<text>();

    EXPECT_TRUE(positions[static_cast<size_t>(btn)].has_value());
    EXPECT_TRUE(drawables[static_cast<size_t>(btn)].has_value());
    EXPECT_TRUE(texts[static_cast<size_t>(btn)].has_value());
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(btn)]->width, 200.f);
    EXPECT_FLOAT_EQ(drawables[static_cast<size_t>(btn)]->height, 50.f);
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->font_size, 35);
}

TEST(Button, multiple_buttons_have_unique_entities)
{
    registry reg;
    register_all_button_components(reg);

    auto b1 = create_button(reg, "btn1", "A", 10.f, 10.f);
    auto b2 = create_button(reg, "btn2", "B", 20.f, 20.f);
    EXPECT_NE(b1, b2);

    auto &positions = reg.get_components<position>();
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(b1)]->x, 10.f);
    EXPECT_FLOAT_EQ(positions[static_cast<size_t>(b2)]->x, 20.f);
}

TEST(Button, correct_text_and_clickable_ids)
{
    registry reg;
    register_all_button_components(reg);

    auto btn = create_button(reg, "unique_btn", "PressMe", 50.f, 50.f);
    auto &texts = reg.get_components<text>();
    auto &clickables = reg.get_components<clickable>();
    auto &hoverables = reg.get_components<hoverable>();

    EXPECT_EQ(texts[static_cast<size_t>(btn)]->content, "PressMe");
    EXPECT_EQ(clickables[static_cast<size_t>(btn)]->id, "unique_btn");
    EXPECT_EQ(hoverables[static_cast<size_t>(btn)]->id, "unique_btn");
}

TEST(Button, custom_colors_applied)
{
    registry reg;
    register_all_button_components(reg);

    Color bg = RED;
    Color txt = BLUE;
    auto btn = create_button(reg, "colored_btn", "Colorful", 
                            5.f, 5.f, 0.f, 150.f, 40.f, bg, txt, 25);
    auto &drawables = reg.get_components<drawable>();
    auto &texts = reg.get_components<text>();

    ASSERT_TRUE(drawables[static_cast<size_t>(btn)].has_value());
    ASSERT_TRUE(texts[static_cast<size_t>(btn)].has_value());

    EXPECT_EQ(drawables[static_cast<size_t>(btn)]->color.r, bg.r);
    EXPECT_EQ(drawables[static_cast<size_t>(btn)]->color.g, bg.g);
    EXPECT_EQ(drawables[static_cast<size_t>(btn)]->color.b, bg.b);
    EXPECT_EQ(drawables[static_cast<size_t>(btn)]->color.a, bg.a);
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->color.r, txt.r);
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->color.g, txt.g);
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->color.b, txt.b);
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->color.a, txt.a);
}

TEST(Button, multiple_buttons_unique)
{
    registry reg;
    register_all_button_components(reg);

    auto b1 = create_button(reg, "id1", "Play", 0.f, 0.f);
    auto b2 = create_button(reg, "id2", "Exit", 10.f, 10.f);

    EXPECT_NE(b1, b2);

    auto &clickables = reg.get_components<clickable>();
    EXPECT_EQ(clickables[static_cast<size_t>(b1)]->id, "id1");
    EXPECT_EQ(clickables[static_cast<size_t>(b2)]->id, "id2");
}

TEST(Button, text_content_fontsize)
{
    registry reg;
    register_all_button_components(reg);

    auto btn = create_button(reg, "Play", "More", 
                            15.f, 15.f, 0.f, 100.f, 40.f, GREEN, WHITE, 42);
    auto &texts = reg.get_components<text>();

    ASSERT_TRUE(texts[static_cast<size_t>(btn)].has_value());
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->content, "More");
    EXPECT_EQ(texts[static_cast<size_t>(btn)]->font_size, 42);
}