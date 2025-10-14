/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** test_background.cpp
*/

#include <gtest/gtest.h>
#include "background.hpp"
#include "components.hpp"
#include "registry.hpp" 

using namespace ecs;
using namespace game::entities;

TEST(Background, has_position_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 100.0f, 200.0f, 800.0f, 600.0f);
    
    auto &positions = reg.get_components<component::position>();
    auto &pos = positions[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(pos.has_value());
    EXPECT_EQ(pos->x, 100.0f);
    EXPECT_EQ(pos->y, 200.0f);
}

TEST(Background, has_drawable_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 1920.0f, 1080.0f);
    
    auto &drawables = reg.get_components<component::drawable>();
    auto &drw = drawables[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(drw.has_value());
    EXPECT_EQ(drw->width, 1920.0f);
    EXPECT_EQ(drw->height, 1080.0f);
}

TEST(Background, has_type_component) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f);
    
    auto &types = reg.get_components<component::type>();
    auto &type_comp = types[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(type_comp.has_value());
    EXPECT_EQ(type_comp->value, component::entity_type::BACKGROUND);
}

TEST(Background, no_sprite_when_empty_path) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, "");
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    EXPECT_FALSE(spr.has_value());
}

TEST(Background, has_sprite_with_image_path) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, 
                                    "assets/background.png");
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(spr.has_value());
    EXPECT_STREQ(spr->image_path.c_str(), "assets/background.png");
}

TEST(Background, sprite_scale_default) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, 
                                    "bg.png");
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(spr.has_value());
    EXPECT_EQ(spr->scale, 1.0f);
}

TEST(Background, sprite_scale_custom) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, 
                                    "bg.png", 2.5f);
    
    auto &sprites = reg.get_components<component::sprite>();
    auto &spr = sprites[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(spr.has_value());
    EXPECT_EQ(spr->scale, 2.5f);
}

TEST(Background, negative_position) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, -50.0f, -100.0f, 800.0f, 600.0f);
    
    auto &positions = reg.get_components<component::position>();
    auto &pos = positions[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(pos.has_value());
    EXPECT_EQ(pos->x, -50.0f);
    EXPECT_EQ(pos->y, -100.0f);
}

TEST(Background, zero_dimensions) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg = create_background(reg, 0.0f, 0.0f, 0.0f, 0.0f);
    
    auto &drawables = reg.get_components<component::drawable>();
    auto &drw = drawables[static_cast<std::size_t>(bg)];
    
    EXPECT_TRUE(drw.has_value());
    EXPECT_EQ(drw->width, 0.0f);
    EXPECT_EQ(drw->height, 0.0f);
}

TEST(Background, multiple_backgrounds) {
    registry reg;
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
    reg.register_component<component::sprite>();
    
    entity_t bg1 = create_background(reg, 0.0f, 0.0f, 800.0f, 600.0f, "bg1.png");
    entity_t bg2 = create_background(reg, 100.0f, 0.0f, 800.0f, 600.0f, "bg2.png");
    
    EXPECT_NE(static_cast<std::size_t>(bg1), static_cast<std::size_t>(bg2));
    
    auto &positions = reg.get_components<component::position>();
    auto &pos1 = positions[static_cast<std::size_t>(bg1)];
    auto &pos2 = positions[static_cast<std::size_t>(bg2)];
    
    EXPECT_TRUE(pos1.has_value() && pos2.has_value());
    EXPECT_EQ(pos1->x, 0.0f);
    EXPECT_EQ(pos2->x, 100.0f);
}

TEST(Background, all_components_independent) {
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
    
    EXPECT_NE(pos1->x, pos2->x);
    EXPECT_NE(drw1->width, drw2->width);
    EXPECT_STRNE(spr1->image_path.c_str(), spr2->image_path.c_str());
    EXPECT_NE(spr1->scale, spr2->scale);
}