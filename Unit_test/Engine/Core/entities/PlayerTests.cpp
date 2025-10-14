/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PlayerTest.cpp
*/

#include <gtest/gtest.h>
#include "player.hpp"
#include "components.hpp"
#include "registry.hpp"
#include "hitbox.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_player_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::previous_position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::controllable>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::hitbox_link>();
}

TEST(Player, has_position_and_previous_position) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 100.f, 200.f, "");

    auto &positions = reg.get_components<component::position>();
    auto &previous = reg.get_components<component::previous_position>();

    EXPECT_TRUE(positions[static_cast<std::size_t>(p)].has_value());
    EXPECT_EQ(positions[static_cast<std::size_t>(p)]->x, 100.f);
    EXPECT_EQ(positions[static_cast<std::size_t>(p)]->y, 200.f);

    EXPECT_TRUE(previous[static_cast<std::size_t>(p)].has_value());
    EXPECT_EQ(previous[static_cast<std::size_t>(p)]->x, 100.f);
    EXPECT_EQ(previous[static_cast<std::size_t>(p)]->y, 200.f);
}

TEST(Player, has_velocity_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &vels = reg.get_components<component::velocity>();
    EXPECT_TRUE(vels[static_cast<std::size_t>(p)].has_value());
    EXPECT_EQ(vels[static_cast<std::size_t>(p)]->vx, 0.f);
    EXPECT_EQ(vels[static_cast<std::size_t>(p)]->vy, 0.f);
}

TEST(Player, has_controllable_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &ctrls = reg.get_components<component::controllable>();
    EXPECT_TRUE(ctrls[static_cast<std::size_t>(p)].has_value());
    EXPECT_EQ(ctrls[static_cast<std::size_t>(p)]->speed, 300.f);
}

TEST(Player, has_health_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &hps = reg.get_components<component::health>();
    EXPECT_TRUE(hps[static_cast<std::size_t>(p)].has_value());
    EXPECT_EQ(hps[static_cast<std::size_t>(p)]->current, 100);
    EXPECT_EQ(hps[static_cast<std::size_t>(p)]->max, 100);
}

TEST(Player, has_type_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &types = reg.get_components<component::type>();
    EXPECT_TRUE(types[static_cast<std::size_t>(p)].has_value());
    EXPECT_EQ(types[static_cast<std::size_t>(p)]->value, component::entity_type::PLAYER);
}

TEST(Player, has_drawable_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &drawables = reg.get_components<component::drawable>();
    EXPECT_TRUE(drawables[static_cast<std::size_t>(p)].has_value());
    EXPECT_EQ(drawables[static_cast<std::size_t>(p)]->width, 30.f);
    EXPECT_EQ(drawables[static_cast<std::size_t>(p)]->height, 30.f);
}

TEST(Player, no_sprite_when_empty_path) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &sprites = reg.get_components<component::sprite>();
    EXPECT_FALSE(sprites[static_cast<std::size_t>(p)].has_value());
}

TEST(Player, has_sprite_when_path_given) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "assets/player.png");

    auto &sprites = reg.get_components<component::sprite>();
    EXPECT_TRUE(sprites[static_cast<std::size_t>(p)].has_value());
    EXPECT_STREQ(sprites[static_cast<std::size_t>(p)]->image_path.c_str(), "assets/player.png");
    EXPECT_EQ(sprites[static_cast<std::size_t>(p)]->scale, 1.f);
}

TEST(Player, has_hitbox_linked) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 10.f, 20.f, "");

    auto &links = reg.get_components<component::hitbox_link>();
    bool found = false;
    for (std::size_t i = 0; i < links.size(); i++) {
        if (links[i] && links[i]->owner.value() == p.value()) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST(PlayerBounds, clamp_position_inside_screen) {
    registry reg;
    register_all_player_components(reg);
    setup_player_bounds_system(reg, 800.f, 600.f);

    entity_t p = create_player(reg, 0.f, 0.f, "");
    auto &positions = reg.get_components<component::position>();

    positions[static_cast<std::size_t>(p)]->x = -50.f;
    positions[static_cast<std::size_t>(p)]->y = -100.f;
    reg.run_systems();

    EXPECT_EQ(positions[static_cast<std::size_t>(p)]->x, 0.f);
    EXPECT_EQ(positions[static_cast<std::size_t>(p)]->y, 0.f);

    positions[static_cast<std::size_t>(p)]->x = 900.f;
    positions[static_cast<std::size_t>(p)]->y = 700.f;
    reg.run_systems();

    EXPECT_EQ(positions[static_cast<std::size_t>(p)]->x, 750.f);
    EXPECT_EQ(positions[static_cast<std::size_t>(p)]->y, 570.f);
}
