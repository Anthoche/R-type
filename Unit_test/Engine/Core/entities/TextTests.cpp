/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** TextTests.cpp
*/

#include <gtest/gtest.h>
#include "text.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_text_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::text>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
}

TEST(Text, has_position_and_text_component) {
    registry reg;
    register_all_text_components(reg);

    Vector2 pos{50.f, 100.f};
    std::string content = "Hello World";
    Color color{255, 0, 0, 255};
    int fontSize = 24;

    entity_t e = create_text(reg, pos, content, color, fontSize);

    auto &positions = reg.get_components<component::position>();
    auto &texts = reg.get_components<component::text>();
    auto &drawables = reg.get_components<component::drawable>();
    auto &types = reg.get_components<component::type>();

    EXPECT_TRUE(positions[static_cast<std::size_t>(e)].has_value());
    EXPECT_EQ(positions[static_cast<std::size_t>(e)]->x, 50.f);
    EXPECT_EQ(positions[static_cast<std::size_t>(e)]->y, 100.f);
    EXPECT_TRUE(texts[static_cast<std::size_t>(e)].has_value());
    EXPECT_STREQ(texts[static_cast<std::size_t>(e)]->content.c_str(), "Hello World");
    EXPECT_EQ(texts[static_cast<std::size_t>(e)]->font_size, 24);
    EXPECT_EQ(texts[static_cast<std::size_t>(e)]->color.r, 255);
    EXPECT_EQ(texts[static_cast<std::size_t>(e)]->color.g, 0);
    EXPECT_EQ(texts[static_cast<std::size_t>(e)]->color.b, 0);
    EXPECT_EQ(texts[static_cast<std::size_t>(e)]->color.a, 255);
    EXPECT_TRUE(drawables[static_cast<std::size_t>(e)].has_value());
    EXPECT_TRUE(types[static_cast<std::size_t>(e)].has_value());
    EXPECT_EQ(types[static_cast<std::size_t>(e)]->value, component::entity_type::TEXT);
}