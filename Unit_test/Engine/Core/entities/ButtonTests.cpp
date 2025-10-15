/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** test_button.cpp
*/

#include <gtest/gtest.h>
#include "button.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

TEST(Button, has_position_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {250.0f, 350.0f};
    Vector2 size = {100.0f, 40.0f};
    
    entity_t btn = create_button(reg, "btn2", "Test", pos, size, BLUE, BLACK);
    
    auto &positions = reg.get_components<component::position>();
    auto &p = positions[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(p.has_value());
    EXPECT_EQ(p->x, 250.0f);
    EXPECT_EQ(p->y, 350.0f);
}

TEST(Button, position_at_origin) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {0.0f, 0.0f};
    Vector2 size = {80.0f, 30.0f};
    
    entity_t btn = create_button(reg, "origin", "Origin", pos, size, GREEN, WHITE);
    
    auto &positions = reg.get_components<component::position>();
    auto &p = positions[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(p.has_value());
    EXPECT_EQ(p->x, 0.0f);
    EXPECT_EQ(p->y, 0.0f);
}

TEST(Button, has_clickable_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {10.0f, 20.0f};
    Vector2 size = {120.0f, 45.0f};
    
    entity_t btn = create_button(reg, "clickable_btn", "Click", pos, size, YELLOW, BLACK);
    
    auto &clickables = reg.get_components<component::clickable>();
    auto &c = clickables[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(c.has_value());
    EXPECT_STREQ(c->id.c_str(), "clickable_btn");
    EXPECT_TRUE(c->enabled);
    EXPECT_FALSE(c->isClicked);
}

TEST(Button, has_hoverable_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {50.0f, 60.0f};
    Vector2 size = {200.0f, 60.0f};
    
    entity_t btn = create_button(reg, "hover_btn", "Hover", pos, size, PURPLE, WHITE);
    
    auto &hoverables = reg.get_components<component::hoverable>();
    auto &h = hoverables[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(h.has_value());
    EXPECT_STREQ(h->id.c_str(), "hover_btn");
    EXPECT_FALSE(h->isHovered);
}

TEST(Button, has_type_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {100.0f, 100.0f};
    Vector2 size = {150.0f, 50.0f};
    
    entity_t btn = create_button(reg, "type_btn", "Type", pos, size, ORANGE, BLACK);
    
    auto &types = reg.get_components<component::type>();
    auto &t = types[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->value, component::entity_type::BUTTON);
}

TEST(Button, has_drawable_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {0.0f, 0.0f};
    Vector2 size = {300.0f, 80.0f};
    Color btnColor = {255, 128, 64, 255};
    
    entity_t btn = create_button(reg, "draw_btn", "Draw", pos, size, btnColor, WHITE);
    
    auto &drawables = reg.get_components<component::drawable>();
    auto &d = drawables[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(d.has_value());
    EXPECT_EQ(d->width, 300.0f);
    EXPECT_EQ(d->height, 80.0f);
    EXPECT_EQ(d->color.r, 255);
    EXPECT_EQ(d->color.g, 128);
    EXPECT_EQ(d->color.b, 64);
    EXPECT_EQ(d->color.a, 255);
}

TEST(Button, negative_position) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {-50.0f, -100.0f};
    Vector2 size = {80.0f, 40.0f};
    
    entity_t btn = create_button(reg, "neg_pos", "Negative", pos, size, BLUE, WHITE);
    
    auto &positions = reg.get_components<component::position>();
    auto &p = positions[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(p.has_value());
    EXPECT_EQ(p->x, -50.0f);
    EXPECT_EQ(p->y, -100.0f);
}

TEST(Button, zero_size) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {10.0f, 10.0f};
    Vector2 size = {0.0f, 0.0f};
    
    entity_t btn = create_button(reg, "zero_size", "Zero", pos, size, RED, WHITE);
    
    auto &drawables = reg.get_components<component::drawable>();
    auto &d = drawables[static_cast<std::size_t>(btn)];
    
    EXPECT_TRUE(d.has_value());
    EXPECT_EQ(d->width, 0.0f);
    EXPECT_EQ(d->height, 0.0f);
}

TEST(Button, multiple_buttons_independent) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos1 = {10.0f, 10.0f};
    Vector2 size1 = {100.0f, 50.0f};
    entity_t btn1 = create_button(reg, "btn_1", "First", pos1, size1, RED, WHITE);
    
    Vector2 pos2 = {200.0f, 100.0f};
    Vector2 size2 = {150.0f, 60.0f};
    entity_t btn2 = create_button(reg, "btn_2", "Second", pos2, size2, BLUE, BLACK);
    
    EXPECT_NE(static_cast<std::size_t>(btn1), static_cast<std::size_t>(btn2));
    
    auto &positions = reg.get_components<component::position>();
    auto &clickables = reg.get_components<component::clickable>();
    auto &texts = reg.get_components<component::text>();
    
    auto &p1 = positions[static_cast<std::size_t>(btn1)];
    auto &p2 = positions[static_cast<std::size_t>(btn2)];
    auto &c1 = clickables[static_cast<std::size_t>(btn1)];
    auto &c2 = clickables[static_cast<std::size_t>(btn2)];
    auto &t1 = texts[static_cast<std::size_t>(btn1)];
    auto &t2 = texts[static_cast<std::size_t>(btn2)];
    
    EXPECT_TRUE(p1.has_value() && p2.has_value());
    EXPECT_NE(p1->x, p2->x);
    EXPECT_STRNE(c1->id.c_str(), c2->id.c_str());
    EXPECT_STRNE(t1->content.c_str(), t2->content.c_str());
}

TEST(Button, unique_ids) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::clickable>();
    reg.register_component<component::hoverable>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::text>();
    
    Vector2 pos = {0.0f, 0.0f};
    Vector2 size = {100.0f, 50.0f};
    
    entity_t btn1 = create_button(reg, "unique_id_1", "Button1", pos, size, RED, WHITE);
    entity_t btn2 = create_button(reg, "unique_id_2", "Button2", pos, size, BLUE, BLACK);
    
    auto &clickables = reg.get_components<component::clickable>();
    auto &c1 = clickables[static_cast<std::size_t>(btn1)];
    auto &c2 = clickables[static_cast<std::size_t>(btn2)];
    
    EXPECT_STRNE(c1->id.c_str(), c2->id.c_str());
}