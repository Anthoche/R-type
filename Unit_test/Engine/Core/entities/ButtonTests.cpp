/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** test_button.cpp
*/

#include <criterion/criterion.h>
#include "button.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

Test(Button, has_position_component) {
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
    
    cr_assert(p.has_value(), "button should have a position component");
    cr_assert_eq(p->x, 250.0f, "x position should be 250.0f");
    cr_assert_eq(p->y, 350.0f, "y position should be 350.0f");
}

Test(Button, position_at_origin) {
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
    
    cr_assert(p.has_value());
    cr_assert_eq(p->x, 0.0f, "x should be 0");
    cr_assert_eq(p->y, 0.0f, "y should be 0");
}

Test(Button, has_clickable_component) {
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
    
    cr_assert(c.has_value(), "button should have a clickable component");
    cr_assert_str_eq(c->id.c_str(), "clickable_btn", "clickable id should match");
    cr_assert_eq(c->enabled, true, "button should be enabled by default");
    cr_assert_eq(c->isClicked, false, "button should not be clicked initially");
}

Test(Button, has_hoverable_component) {
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
    
    cr_assert(h.has_value(), "button should have a hoverable component");
    cr_assert_str_eq(h->id.c_str(), "hover_btn", "hoverable id should match");
    cr_assert_eq(h->isHovered, false, "button should not be hovered initially");
}

Test(Button, has_type_component) {
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
    
    cr_assert(t.has_value(), "button should have a type component");
    cr_assert_eq(t->value, component::entity_type::BUTTON, "type should be BUTTON");
}

Test(Button, has_drawable_component) {
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
    
    cr_assert(d.has_value(), "button should have a drawable component");
    cr_assert_eq(d->width, 300.0f, "width should be 300.0f");
    cr_assert_eq(d->height, 80.0f, "height should be 80.0f");
    cr_assert_eq(d->color.r, 255, "red channel should match");
    cr_assert_eq(d->color.g, 128, "green channel should match");
    cr_assert_eq(d->color.b, 64, "blue channel should match");
    cr_assert_eq(d->color.a, 255, "alpha channel should match");
}

Test(Button, negative_position) {
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
    
    cr_assert(p.has_value());
    cr_assert_eq(p->x, -50.0f, "negative x should be allowed");
    cr_assert_eq(p->y, -100.0f, "negative y should be allowed");
}

Test(Button, zero_size) {
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
    
    cr_assert(d.has_value());
    cr_assert_eq(d->width, 0.0f, "zero width should be allowed");
    cr_assert_eq(d->height, 0.0f, "zero height should be allowed");
}

Test(Button, multiple_buttons_independent) {
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
    
    cr_assert_neq(static_cast<std::size_t>(btn1), static_cast<std::size_t>(btn2),
                  "buttons should have different IDs");
    
    auto &positions = reg.get_components<component::position>();
    auto &clickables = reg.get_components<component::clickable>();
    auto &texts = reg.get_components<component::text>();
    
    auto &p1 = positions[static_cast<std::size_t>(btn1)];
    auto &p2 = positions[static_cast<std::size_t>(btn2)];
    auto &c1 = clickables[static_cast<std::size_t>(btn1)];
    auto &c2 = clickables[static_cast<std::size_t>(btn2)];
    auto &t1 = texts[static_cast<std::size_t>(btn1)];
    auto &t2 = texts[static_cast<std::size_t>(btn2)];
    
    cr_assert(p1.has_value() && p2.has_value());
    cr_assert_neq(p1->x, p2->x, "positions should differ");
    cr_assert_str_neq(c1->id.c_str(), c2->id.c_str(), "IDs should differ");
    cr_assert_str_neq(t1->content.c_str(), t2->content.c_str(), "text should differ");
}

Test(Button, unique_ids) {
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
    
    cr_assert_str_neq(c1->id.c_str(), c2->id.c_str(), 
                     "clickable IDs should be different");
}