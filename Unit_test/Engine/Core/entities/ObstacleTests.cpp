/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ObstacleTest.cpp
*/

#include <gtest/gtest.h>
#include "obstacle.hpp"
#include "components.hpp"
#include "registry.hpp"
#include "hitbox.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_obstacle_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::previous_position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::health>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::hitbox_link>();
    reg.register_component<component::hitbox_link>();
}

TEST(Obstacle, has_position_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 100.f, 200.f, "");

    auto &positions = reg.get_components<component::position>();
    EXPECT_TRUE(positions[static_cast<std::size_t>(obs)].has_value());
    EXPECT_EQ(positions[static_cast<std::size_t>(obs)]->x, 100.f);
    EXPECT_EQ(positions[static_cast<std::size_t>(obs)]->y, 200.f);
}

TEST(Obstacle, has_previous_position_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 100.f, 200.f, "");

    auto &prevPos = reg.get_components<component::previous_position>();
    EXPECT_TRUE(prevPos[static_cast<std::size_t>(obs)].has_value());
    EXPECT_EQ(prevPos[static_cast<std::size_t>(obs)]->x, 100.f);
    EXPECT_EQ(prevPos[static_cast<std::size_t>(obs)]->y, 200.f);
}

TEST(Obstacle, has_health_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &healths = reg.get_components<component::health>();
    EXPECT_TRUE(healths[static_cast<std::size_t>(obs)].has_value());
    EXPECT_EQ(healths[static_cast<std::size_t>(obs)]->current, 9999);
    EXPECT_EQ(healths[static_cast<std::size_t>(obs)]->max, 9999);
}

TEST(Obstacle, has_type_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &types = reg.get_components<component::type>();
    EXPECT_TRUE(types[static_cast<std::size_t>(obs)].has_value());
    EXPECT_EQ(types[static_cast<std::size_t>(obs)]->value, component::entity_type::OBSTACLE);
}

TEST(Obstacle, no_sprite_when_empty_path) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &sprites = reg.get_components<component::sprite>();
    EXPECT_FALSE(sprites[static_cast<std::size_t>(obs)].has_value());
}

TEST(Obstacle, has_sprite_when_path_given) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "assets/obstacle.png");

    auto &sprites = reg.get_components<component::sprite>();
    EXPECT_TRUE(sprites[static_cast<std::size_t>(obs)].has_value());
    EXPECT_STREQ(sprites[static_cast<std::size_t>(obs)]->image_path.c_str(), "assets/obstacle.png");
    EXPECT_EQ(sprites[static_cast<std::size_t>(obs)]->scale, 1.0f);
}

TEST(Obstacle, has_hitbox_linked) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &links = reg.get_components<component::hitbox_link>();

    bool found = false;
    for (std::size_t i = 0; i < links.size(); ++i) {
        if (links[i] && links[i]->owner.value() == obs.value()) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}
