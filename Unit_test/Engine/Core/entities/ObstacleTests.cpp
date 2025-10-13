/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ObstacleTest.cpp
*/

#include <criterion/criterion.h>
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

Test(Obstacle, has_position_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 100.f, 200.f, "");

    auto &positions = reg.get_components<component::position>();
    cr_assert(positions[static_cast<std::size_t>(obs)].has_value());
    cr_assert_eq(positions[static_cast<std::size_t>(obs)]->x, 100.f);
    cr_assert_eq(positions[static_cast<std::size_t>(obs)]->y, 200.f);
}

Test(Obstacle, has_previous_position_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 100.f, 200.f, "");

    auto &prevPos = reg.get_components<component::previous_position>();
    cr_assert(prevPos[static_cast<std::size_t>(obs)].has_value());
    cr_assert_eq(prevPos[static_cast<std::size_t>(obs)]->x, 100.f);
    cr_assert_eq(prevPos[static_cast<std::size_t>(obs)]->y, 200.f);
}

Test(Obstacle, has_health_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &healths = reg.get_components<component::health>();
    cr_assert(healths[static_cast<std::size_t>(obs)].has_value());
    cr_assert_eq(healths[static_cast<std::size_t>(obs)]->current, 9999);
    cr_assert_eq(healths[static_cast<std::size_t>(obs)]->max, 9999);
}

Test(Obstacle, has_type_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &types = reg.get_components<component::type>();
    cr_assert(types[static_cast<std::size_t>(obs)].has_value());
    cr_assert_eq(types[static_cast<std::size_t>(obs)]->value, component::entity_type::OBSTACLE);
}

Test(Obstacle, has_collision_box_component) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &boxes = reg.get_components<component::collision_box>();
    cr_assert(boxes[static_cast<std::size_t>(obs)].has_value());
    cr_assert_eq(boxes[static_cast<std::size_t>(obs)]->width, 60.f);
    cr_assert_eq(boxes[static_cast<std::size_t>(obs)]->height, 60.f);
}

Test(Obstacle, no_sprite_when_empty_path) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "");

    auto &sprites = reg.get_components<component::sprite>();
    cr_assert(!sprites[static_cast<std::size_t>(obs)].has_value(),
              "Obstacle should not have sprite if no path is given");
}

Test(Obstacle, has_sprite_when_path_given) {
    registry reg;
    register_all_obstacle_components(reg);

    entity_t obs = create_obstacle(reg, 0.f, 0.f, "assets/obstacle.png");

    auto &sprites = reg.get_components<component::sprite>();
    cr_assert(sprites[static_cast<std::size_t>(obs)].has_value());
    cr_assert_str_eq(sprites[static_cast<std::size_t>(obs)]->image_path.c_str(), "assets/obstacle.png");
    cr_assert_eq(sprites[static_cast<std::size_t>(obs)]->scale, 1.0f);
}

Test(Obstacle, has_hitbox_linked) {
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
    cr_assert(found, "Obstacle should have a hitbox entity linked to it");
}
