/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** RandomElementTest.cpp
*/

#include <gtest/gtest.h>
#include "random_element.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_random_element_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::audio>();
}

TEST(RandomElement, has_position_component) {
    registry reg;
    register_all_random_element_components(reg);

    entity_t e = create_random_element(reg, 10.f, 20.f, 100.f, 50.f, "", "", 0.f, false, false);

    auto &positions = reg.get_components<component::position>();
    EXPECT_TRUE(positions[static_cast<std::size_t>(e)].has_value());
    EXPECT_EQ(positions[static_cast<std::size_t>(e)]->x, 10.f);
    EXPECT_EQ(positions[static_cast<std::size_t>(e)]->y, 20.f);
}

TEST(RandomElement, has_drawable_component) {
    registry reg;
    register_all_random_element_components(reg);

    entity_t e = create_random_element(reg, 0.f, 0.f, 200.f, 150.f, "", "", 0.f, false, false);

    auto &drawables = reg.get_components<component::drawable>();
    EXPECT_TRUE(drawables[static_cast<std::size_t>(e)].has_value());
    EXPECT_EQ(drawables[static_cast<std::size_t>(e)]->width, 200.f);
    EXPECT_EQ(drawables[static_cast<std::size_t>(e)]->height, 150.f);
}

TEST(RandomElement, no_sprite_when_empty_path) {
    registry reg;
    register_all_random_element_components(reg);

    entity_t e = create_random_element(reg, 0.f, 0.f, 10.f, 10.f, "", "", 0.f, false, false);

    auto &sprites = reg.get_components<component::sprite>();
    EXPECT_FALSE(sprites[static_cast<std::size_t>(e)].has_value());
}

TEST(RandomElement, has_sprite_when_path_given) {
    registry reg;
    register_all_random_element_components(reg);

    entity_t e = create_random_element(reg, 0.f, 0.f, 10.f, 10.f, "assets/img.png", "", 0.f, false, false);

    auto &sprites = reg.get_components<component::sprite>();
    EXPECT_TRUE(sprites[static_cast<std::size_t>(e)].has_value());
    EXPECT_STREQ(sprites[static_cast<std::size_t>(e)]->image_path.c_str(), "assets/img.png");
}

TEST(RandomElement, no_audio_when_empty_path) {
    registry reg;
    register_all_random_element_components(reg);

    entity_t e = create_random_element(reg, 0.f, 0.f, 10.f, 10.f, "", "", 0.f, false, false);

    auto &audios = reg.get_components<component::audio>();
    EXPECT_FALSE(audios[static_cast<std::size_t>(e)].has_value());
}

TEST(RandomElement, has_audio_when_path_given) {
    registry reg;
    register_all_random_element_components(reg);

    entity_t e = create_random_element(reg, 0.f, 0.f, 10.f, 10.f, "", "assets/sound.ogg", 0.5f, true, true);

    auto &audios = reg.get_components<component::audio>();
    EXPECT_TRUE(audios[static_cast<std::size_t>(e)].has_value());
    EXPECT_STREQ(audios[static_cast<std::size_t>(e)]->sound_path.c_str(), "assets/sound.ogg");
    EXPECT_EQ(audios[static_cast<std::size_t>(e)]->volume, 0.5f);
    EXPECT_TRUE(audios[static_cast<std::size_t>(e)]->loop);
    EXPECT_TRUE(audios[static_cast<std::size_t>(e)]->autoplay);
}
