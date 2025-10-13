/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PlayerTest.cpp
*/

#include <criterion/criterion.h>
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

Test(Player, has_position_and_previous_position) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 100.f, 200.f, "");

    auto &positions = reg.get_components<component::position>();
    auto &previous = reg.get_components<component::previous_position>();

    cr_assert(positions[static_cast<std::size_t>(p)].has_value());
    cr_assert_eq(positions[static_cast<std::size_t>(p)]->x, 100.f);
    cr_assert_eq(positions[static_cast<std::size_t>(p)]->y, 200.f);

    cr_assert(previous[static_cast<std::size_t>(p)].has_value());
    cr_assert_eq(previous[static_cast<std::size_t>(p)]->x, 100.f);
    cr_assert_eq(previous[static_cast<std::size_t>(p)]->y, 200.f);
}

Test(Player, has_velocity_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &vels = reg.get_components<component::velocity>();
    cr_assert(vels[static_cast<std::size_t>(p)].has_value());
    cr_assert_eq(vels[static_cast<std::size_t>(p)]->vx, 0.f);
    cr_assert_eq(vels[static_cast<std::size_t>(p)]->vy, 0.f);
}

Test(Player, has_controllable_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &ctrls = reg.get_components<component::controllable>();
    cr_assert(ctrls[static_cast<std::size_t>(p)].has_value());
    cr_assert_eq(ctrls[static_cast<std::size_t>(p)]->speed, 300.f);
}

Test(Player, has_health_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &hps = reg.get_components<component::health>();
    cr_assert(hps[static_cast<std::size_t>(p)].has_value());
    cr_assert_eq(hps[static_cast<std::size_t>(p)]->current, 100);
    cr_assert_eq(hps[static_cast<std::size_t>(p)]->max, 100);
}

Test(Player, has_type_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &types = reg.get_components<component::type>();
    cr_assert(types[static_cast<std::size_t>(p)].has_value());
    cr_assert_eq(types[static_cast<std::size_t>(p)]->value, component::entity_type::PLAYER);
}

Test(Player, has_drawable_component) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &drawables = reg.get_components<component::drawable>();
    cr_assert(drawables[static_cast<std::size_t>(p)].has_value());
    cr_assert_eq(drawables[static_cast<std::size_t>(p)]->width, 30.f);
    cr_assert_eq(drawables[static_cast<std::size_t>(p)]->height, 30.f);
}

Test(Player, no_sprite_when_empty_path) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "");

    auto &sprites = reg.get_components<component::sprite>();
    cr_assert(!sprites[static_cast<std::size_t>(p)].has_value(),
              "player should not have sprite with empty path");
}

Test(Player, has_sprite_when_path_given) {
    registry reg;
    register_all_player_components(reg);

    entity_t p = create_player(reg, 0.f, 0.f, "assets/player.png");

    auto &sprites = reg.get_components<component::sprite>();
    cr_assert(sprites[static_cast<std::size_t>(p)].has_value());
    cr_assert_str_eq(sprites[static_cast<std::size_t>(p)]->image_path.c_str(), "assets/player.png");
    cr_assert_eq(sprites[static_cast<std::size_t>(p)]->scale, 1.f);
}

Test(Player, has_hitbox_linked) {
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
    cr_assert(found, "player should have a hitbox linked to it");
}

Test(PlayerBounds, clamp_position_inside_screen) {
    registry reg;
    register_all_player_components(reg);
    setup_player_bounds_system(reg, 800.f, 600.f);

    entity_t p = create_player(reg, 0.f, 0.f, "");
    auto &positions = reg.get_components<component::position>();

    positions[static_cast<std::size_t>(p)]->x = -50.f;
    positions[static_cast<std::size_t>(p)]->y = -100.f;
    reg.run_systems();

    cr_assert_eq(positions[static_cast<std::size_t>(p)]->x, 0.f);
    cr_assert_eq(positions[static_cast<std::size_t>(p)]->y, 0.f);

    positions[static_cast<std::size_t>(p)]->x = 900.f;
    positions[static_cast<std::size_t>(p)]->y = 700.f;
    reg.run_systems();

    cr_assert_eq(positions[static_cast<std::size_t>(p)]->x, 750.f, "right bound = screen_width - 50");
    cr_assert_eq(positions[static_cast<std::size_t>(p)]->y, 570.f, "bottom bound = screen_height - 30");
}
