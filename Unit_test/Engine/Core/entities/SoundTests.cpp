/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SoundTest.cpp
*/

#include <gtest/gtest.h>
#include "sound.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_sound_components(registry &reg) {
    reg.register_component<component::type>();
    reg.register_component<component::audio>();
}

TEST(Sound, has_type_sound) {
    registry reg;
    register_all_sound_components(reg);

    entity_t s = create_sound(reg, "assets/sound.ogg", 0.8f, true, false);

    auto &types = reg.get_components<component::type>();
    EXPECT_TRUE(types[static_cast<std::size_t>(s)].has_value());
    EXPECT_EQ(types[static_cast<std::size_t>(s)]->value, component::entity_type::SOUND);
}

TEST(Sound, has_audio_component) {
    registry reg;
    register_all_sound_components(reg);

    entity_t s = create_sound(reg, "assets/sound.ogg", 0.8f, true, false);

    auto &audios = reg.get_components<component::audio>();
    EXPECT_TRUE(audios[static_cast<std::size_t>(s)].has_value());
    EXPECT_STREQ(audios[static_cast<std::size_t>(s)]->sound_path.c_str(), "assets/sound.ogg");
    EXPECT_EQ(audios[static_cast<std::size_t>(s)]->volume, 0.8f);
    EXPECT_TRUE(audios[static_cast<std::size_t>(s)]->loop);
    EXPECT_FALSE(audios[static_cast<std::size_t>(s)]->autoplay);
}

TEST(Sound, audio_flags) {
    registry reg;
    register_all_sound_components(reg);

    entity_t s = create_sound(reg, "sfx.wav", 0.5f, false, true);

    auto &audios = reg.get_components<component::audio>();
    EXPECT_TRUE(audios[static_cast<std::size_t>(s)].has_value());
    EXPECT_FALSE(audios[static_cast<std::size_t>(s)]->loop);
    EXPECT_TRUE(audios[static_cast<std::size_t>(s)]->autoplay);
}
