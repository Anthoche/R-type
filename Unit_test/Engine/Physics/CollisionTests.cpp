/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** CollisionTest.cpp
*/

#include <criterion/criterion.h>
#include "GameScene.hpp"
#include "hitbox.hpp"
#include "player.hpp"
#include "enemy.hpp"
#include "obstacle.hpp"
#include "Collision.hpp"

using namespace ecs;
using namespace game::entities;
using namespace game::scene::collision;

// Helper pour enregistrer les composants nécessaires
static void register_collision_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::previous_position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::hitbox_link>();
    reg.register_component<component::type>();
    reg.register_component<component::health>();
    reg.register_component<component::drawable>();
}

Test(Collision, find_hitbox_of_and_player_hitbox) {
    registry reg;
    register_collision_components(reg);

    entity_t player = create_player(reg, 100.f, 100.f, "");
    entity_t playerHitbox = create_hitbox_for(reg, player);

    cr_assert_eq(find_hitbox_of(*(game::scene::GameScene*)nullptr, player).value(), playerHitbox.value());
    cr_assert_eq(find_player_hitbox(*(game::scene::GameScene*)nullptr).value(), playerHitbox.value());
}

Test(Collision, overlap_aabb_true_and_false) {
    cr_assert(overlap_aabb(0, 10, 0, 10, 5, 15, 5, 15));
    cr_assert_not(overlap_aabb(0, 5, 0, 5, 6, 10, 6, 10));
}

Test(Collision, is_blocked_test) {
    registry reg;
    register_collision_components(reg);

    entity_t player = create_player(reg, 0.f, 0.f, "");

    component::position playerPos{50.f, 50.f};
    component::collision_box playerBox{30.f, 30.f};
    component::position obstaclePos{50.f, 50.f};
    component::collision_box obstacleBox{30.f, 30.f};

    cr_assert(is_blocked(*(game::scene::GameScene*)nullptr, 50.f, 50.f, playerPos, playerBox));
    cr_assert_not(is_blocked(*(game::scene::GameScene*)nullptr, 0.f, 0.f, playerPos, playerBox));
}

Test(Collision, resolve_collision_obstacle) {
    registry reg;
    register_collision_components(reg);

    entity_t player = create_player(reg, 0.f, 0.f, "");
    entity_t hitbox = create_hitbox_for(reg, player);

    component::position playerPos{5.f, 0.f};
    component::collision_box playerBox{20.f, 20.f};
    component::position obsPos{15.f, 0.f};
    component::collision_box obsBox{10.f, 10.f};
    float prevX = 0.f, prevY = 0.f;

    resolve_collision(*(game::scene::GameScene*)nullptr, player, playerPos, playerBox, obsPos, obsBox, prevX, prevY);

    cr_assert(playerPos.x < obsPos.x, "Player should be pushed left of obstacle");
}

Test(Collision, player_hits_enemy_sets_health_zero) {
    registry reg;
    register_collision_components(reg);

    entity_t player = create_player(reg, 0.f, 0.f, "");
    entity_t enemy = create_enemy(reg, 0.f, 0.f, "");

    component::health &playerHealth = *reg.get_components<component::health>()[player.value()];

    handle_entity_collisions(*(game::scene::GameScene*)nullptr, player);

    cr_assert_eq(playerHealth.current, 0, "Player health should be 0 after colliding with enemy");
}
