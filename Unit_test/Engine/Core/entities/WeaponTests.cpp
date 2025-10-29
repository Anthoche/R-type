/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WeaponTests.cpp
*/

#include <gtest/gtest.h>
#include "registry.hpp"
#include "weapon.hpp"
#include "components.hpp"

using namespace ecs;
using namespace game::entities;
using namespace component;

namespace {

void register_weapon_components(registry &reg)
{
    reg.register_component<position>();
    reg.register_component<previous_position>();
    reg.register_component<velocity>();
    reg.register_component<type>();
    reg.register_component<collision_box>();
    reg.register_component<drawable>();
    reg.register_component<sprite>();
    reg.register_component<model3D>();
    reg.register_component<hitbox_link>();
}

} // namespace

TEST(Weapon, creates_required_components)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, 4.f, 5.f, 6.f, 12.f, 8.f, 3.f, "tex.png", "model.glb");

    const auto idx = static_cast<std::size_t>(weapon);
    auto &positions = reg.get_components<position>();
    auto &previous = reg.get_components<previous_position>();
    auto &velocities = reg.get_components<velocity>();
    auto &types = reg.get_components<type>();
    auto &collisions = reg.get_components<collision_box>();
    auto &drawables = reg.get_components<drawable>();

    ASSERT_TRUE(positions[idx].has_value());
    ASSERT_TRUE(previous[idx].has_value());
    ASSERT_TRUE(velocities[idx].has_value());
    ASSERT_TRUE(types[idx].has_value());
    ASSERT_TRUE(collisions[idx].has_value());
    ASSERT_TRUE(drawables[idx].has_value());
}

TEST(Weapon, previous_position_equals_initial_coordinates)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, -2.5f, 7.5f, 1.f, 10.f, 5.f, 4.f, "", "");
    const auto idx = static_cast<std::size_t>(weapon);

    auto &positions = reg.get_components<position>();
    auto &previous = reg.get_components<previous_position>();

    ASSERT_TRUE(positions[idx].has_value());
    ASSERT_TRUE(previous[idx].has_value());
    EXPECT_FLOAT_EQ(previous[idx]->x, positions[idx]->x);
    EXPECT_FLOAT_EQ(previous[idx]->y, positions[idx]->y);
    EXPECT_FLOAT_EQ(previous[idx]->z, positions[idx]->z);
}

TEST(Weapon, collision_box_uses_provided_dimensions)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, 0.f, 0.f, 0.f, 18.f, 9.f, 6.f, "", "");
    const auto idx = static_cast<std::size_t>(weapon);
    auto &collisions = reg.get_components<collision_box>();

    ASSERT_TRUE(collisions[idx].has_value());
    EXPECT_FLOAT_EQ(collisions[idx]->width, 18.f);
    EXPECT_FLOAT_EQ(collisions[idx]->height, 9.f);
    EXPECT_FLOAT_EQ(collisions[idx]->depth, 6.f);
}

TEST(Weapon, drawable_matches_provided_dimensions)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, 0.f, 0.f, 0.f, 14.f, 11.f, 7.f, "", "");
    const auto idx = static_cast<std::size_t>(weapon);
    auto &drawables = reg.get_components<drawable>();

    ASSERT_TRUE(drawables[idx].has_value());
    EXPECT_FLOAT_EQ(drawables[idx]->width, 14.f);
    EXPECT_FLOAT_EQ(drawables[idx]->height, 11.f);
    EXPECT_FLOAT_EQ(drawables[idx]->depth, 7.f);
}

TEST(Weapon, drawable_color_is_orange)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, 0.f, 0.f, 0.f, 12.f, 12.f, 12.f, "", "");
    const auto idx = static_cast<std::size_t>(weapon);
    auto &drawables = reg.get_components<drawable>();

    ASSERT_TRUE(drawables[idx].has_value());
    const Color expected = ORANGE;
    EXPECT_EQ(drawables[idx]->color.r, expected.r);
    EXPECT_EQ(drawables[idx]->color.g, expected.g);
    EXPECT_EQ(drawables[idx]->color.b, expected.b);
    EXPECT_EQ(drawables[idx]->color.a, expected.a);
}

TEST(Weapon, velocity_starts_zero)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, 0.f, 0.f, 0.f, 6.f, 6.f, 6.f, "", "");
    const auto idx = static_cast<std::size_t>(weapon);
    auto &velocities = reg.get_components<velocity>();

    ASSERT_TRUE(velocities[idx].has_value());
    EXPECT_FLOAT_EQ(velocities[idx]->vx, 0.f);
    EXPECT_FLOAT_EQ(velocities[idx]->vy, 0.f);
    EXPECT_FLOAT_EQ(velocities[idx]->vz, 0.f);
}

TEST(Weapon, sprite_presence_matches_image_path)
{
    registry reg;
    register_weapon_components(reg);

    auto weaponWithSprite = create_weapon(reg, 0.f, 0.f, 0.f, 5.f, 5.f, 5.f, "assets/weapon.png", "");
    auto weaponWithoutSprite = create_weapon(reg, 1.f, 1.f, 1.f, 5.f, 5.f, 5.f, "", "");

    auto &sprites = reg.get_components<sprite>();

    ASSERT_TRUE(sprites[weaponWithSprite.value()].has_value());
    EXPECT_EQ(sprites[weaponWithSprite.value()]->image_path, "assets/weapon.png");
    EXPECT_FALSE(sprites[weaponWithoutSprite.value()].has_value());
}

TEST(Weapon, model_presence_matches_model_path)
{
    registry reg;
    register_weapon_components(reg);

    auto weaponWithModel = create_weapon(reg, 0.f, 0.f, 0.f, 5.f, 5.f, 5.f, "", "models/weapon.glb");
    auto weaponWithoutModel = create_weapon(reg, 1.f, 1.f, 1.f, 5.f, 5.f, 5.f, "", "");

    auto &models = reg.get_components<model3D>();

    ASSERT_TRUE(models[weaponWithModel.value()].has_value());
    EXPECT_EQ(models[weaponWithModel.value()]->model_path, "models/weapon.glb");
    EXPECT_FALSE(models[weaponWithoutModel.value()].has_value());
}

TEST(Weapon, hitbox_linked_with_matching_position)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, 3.f, -1.f, 2.f, 9.f, 4.f, 3.f, "", "");
    auto &links = reg.get_components<hitbox_link>();
    auto &positions = reg.get_components<position>();

    ecs::entity_t hitbox = weapon;
    bool found = false;
    for (std::size_t i = 0; i < links.size(); ++i) {
        if (links[i] && links[i]->owner == weapon) {
            hitbox = reg.entity_from_index(i);
            found = true;
            break;
        }
    }

    ASSERT_TRUE(found);
    ASSERT_NE(static_cast<std::size_t>(hitbox), static_cast<std::size_t>(weapon));
    ASSERT_TRUE(positions[weapon.value()].has_value());
    ASSERT_TRUE(positions[hitbox.value()].has_value());

    EXPECT_FLOAT_EQ(positions[hitbox.value()]->x, positions[weapon.value()]->x);
    EXPECT_FLOAT_EQ(positions[hitbox.value()]->y, positions[weapon.value()]->y);
    EXPECT_FLOAT_EQ(positions[hitbox.value()]->z, positions[weapon.value()]->z);
}

TEST(Weapon, hitbox_collision_matches_weapon_drawable)
{
    registry reg;
    register_weapon_components(reg);

    auto weapon = create_weapon(reg, 0.f, 0.f, 0.f, 17.f, 13.f, 5.f, "", "");

    auto &links = reg.get_components<hitbox_link>();
    auto &collisions = reg.get_components<collision_box>();
    auto &drawables = reg.get_components<drawable>();

    ecs::entity_t hitbox = weapon;
    bool found = false;
    for (std::size_t i = 0; i < links.size(); ++i) {
        if (links[i] && links[i]->owner == weapon) {
            hitbox = reg.entity_from_index(i);
            found = true;
            break;
        }
    }

    ASSERT_TRUE(found);
    ASSERT_TRUE(drawables[weapon.value()].has_value());
    ASSERT_TRUE(collisions[weapon.value()].has_value());
    ASSERT_TRUE(collisions[hitbox.value()].has_value());

    EXPECT_FLOAT_EQ(collisions[hitbox.value()]->width, drawables[weapon.value()]->width);
    EXPECT_FLOAT_EQ(collisions[hitbox.value()]->height, drawables[weapon.value()]->height);
    EXPECT_FLOAT_EQ(collisions[hitbox.value()]->depth, drawables[weapon.value()]->depth);
}