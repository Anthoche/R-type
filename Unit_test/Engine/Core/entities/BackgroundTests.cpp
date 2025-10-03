/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** test_background.cpp
*/

#include <criterion/criterion.h>
#include "background.hpp"
#include "components.hpp"
#include "registry.hpp" 

using namespace ecs;
using namespace game::entities;

Test(Background, has_position_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 100.0f, 200.0f, 800.0f, 600.0f);
    
    auto &positions = reg.get_components<component::position>();
    auto &pos = positions[static_cast<std::size_t>(bg)];
    
    cr_assert(pos.has_value(), "background should have a position component");
    cr_assert_eq(pos->x, 100.0f, "x position should be 100.0f");
    cr_assert_eq(pos->y, 200.0f, "y position should be 200.0f");
}

Test(Background, has_drawable_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 1920.0f, 1080.0f);
    
    auto &drawables = reg.get_components<component::drawable>();
    auto &drw = drawables[static_cast<std::size_t>(bg)];
    
    cr_assert(drw.has_value(), "background should have a drawable component");
    cr_assert_eq(drw->width, 1920.0f, "width should be 1920.0f");
    cr_assert_eq(drw->height, 1080.0f, "height should be 1080.0f");
}

Test(Background, has_type_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f);
    
    auto &types = reg.get_components<component::type>();
    auto &type_comp = types[static_cast<std::size_t>(bg)];
    
    cr_assert(type_comp.has_value(), "background should have a type component");
    cr_assert_eq(type_comp->value, component::entity_type::BACKGROUND, 
                 "type should be BACKGROUND");
}

Test(Background, no_sprite_when_empty_path) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, "");
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    cr_assert(!spr.has_value(), 
              "background should not have sprite with empty path");
}

Test(Background, has_sprite_with_image_path) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, 
                                    "assets/background.png");
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    cr_assert(spr.has_value(), "background should have sprite component");
    cr_assert_str_eq(spr->image_path.c_str(), "assets/background.png", 
                     "sprite path should match");
}

Test(Background, sprite_scale_default) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, 
                                    "bg.png");
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    cr_assert(spr.has_value());
    cr_assert_eq(spr->scale, 1.0f, "default scale should be 1.0f");
}

Test(Background, sprite_scale_custom) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, 
                                    "bg.png", 2.5f);
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    cr_assert(spr.has_value());
    cr_assert_eq(spr->scale, 2.5f, "custom scale should be 2.5f");
}

Test(Background, negative_position) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, -50.0f, -100.0f, 800.0f, 600.0f);
    
    auto &positions = reg.get_components<component::position>();
    auto &pos = positions[static_cast<std::size_t>(bg)];
    
    cr_assert(pos.has_value());
    cr_assert_eq(pos->x, -50.0f, "negative x should be allowed");
    cr_assert_eq(pos->y, -100.0f, "negative y should be allowed");
}

Test(Background, zero_dimensions) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 0.0f, 0.0f);
    
    auto &drawables = reg.get_components<component::drawable>();
    auto &drw = drawables[static_cast<std::size_t>(bg)];
    
    cr_assert(drw.has_value());
    cr_assert_eq(drw->width, 0.0f, "zero width should be allowed");
    cr_assert_eq(drw->height, 0.0f, "zero height should be allowed");
}

Test(Background, multiple_backgrounds) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg1 = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, "bg1.png");
    entity_t bg2 = create_background(reg, 100.0f, 0.0f, 800.0f, 600.0f, "bg2.png");
    
    cr_assert_neq(static_cast<std::size_t>(bg1), static_cast<std::size_t>(bg2), 
                  "different backgrounds should have different IDs");
    
    auto &positions = reg.get_components<component::position>();
    auto &pos1 = positions[static_cast<std::size_t>(bg1)];
    auto &pos2 = positions[static_cast<std::size_t>(bg2)];
    
    cr_assert(pos1.has_value() && pos2.has_value());
    cr_assert_eq(pos1->x, 0.0f);
    cr_assert_eq(pos2->x, 100.0f);
}

Test(Background, all_components_independent) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg1 = create_background(reg, 0.0f, 0.0f, 400.0f, 300.0f, "a.png", 1.0f);
    entity_t bg2 = create_background(reg, 50.0f, 50.0f, 800.0f, 600.0f, "b.png", 2.0f);
    
    auto &positions = reg.get_components<component::position>();
    auto &drawables = reg.get_components<component::drawable>();
    auto &sprites = reg.get_components<component::sprite>();
    
    auto &pos1 = positions[static_cast<std::size_t>(bg1)];
    auto &pos2 = positions[static_cast<std::size_t>(bg2)];
    auto &drw1 = drawables[static_cast<std::size_t>(bg1)];
    auto &drw2 = drawables[static_cast<std::size_t>(bg2)];
    auto &spr1 = sprites[static_cast<std::size_t>(bg1)];
    auto &spr2 = sprites[static_cast<std::size_t>(bg2)];
    
    cr_assert_neq(pos1->x, pos2->x);
    cr_assert_neq(drw1->width, drw2->width);
    cr_assert_str_neq(spr1->image_path.c_str(), spr2->image_path.c_str());
    cr_assert_neq(spr1->scale, spr2->scale);
}