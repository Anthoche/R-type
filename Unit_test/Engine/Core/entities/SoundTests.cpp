/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SoundTest.cpp
*/

#include <criterion/criterion.h>
#include "sound.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_sound_components(registry &reg) {
    reg.register_component<component::type>();
    reg.register_component<component::audio>();
}

Test(Sound, has_type_sound) {
    registry reg;
    register_all_sound_components(reg);

    entity_t s = create_sound(reg, "assets/sound.ogg", 0.8f, true, false);

    auto &types = reg.get_components<component::type>();
    cr_assert(types[static_cast<std::size_t>(s)].has_value());
    cr_assert_eq(types[static_cast<std::size_t>(s)]->value, component::entity_type::SOUND);
}

Test(Sound, has_audio_component) {
    registry reg;
    register_all_sound_components(reg);

    entity_t s = create_sound(reg, "assets/sound.ogg", 0.8f, true, false);

    auto &audios = reg.get_components<component::audio>();
    cr_assert(audios[static_cast<std::size_t>(s)].has_value());
    cr_assert_str_eq(audios[static_cast<std::size_t>(s)]->sound_path.c_str(), "assets/sound.ogg");
    cr_assert_eq(audios[static_cast<std::size_t>(s)]->volume, 0.8f);
    cr_assert(audios[static_cast<std::size_t>(s)]->loop);
    cr_assert(!audios[static_cast<std::size_t>(s)]->autoplay);
}

Test(Sound, audio_flags) {
    registry reg;
    register_all_sound_components(reg);

    entity_t s = create_sound(reg, "sfx.wav", 0.5f, false, true);

    auto &audios = reg.get_components<component::audio>();
    cr_assert(audios[static_cast<std::size_t>(s)].has_value());
    cr_assert(!audios[static_cast<std::size_t>(s)]->loop);
    cr_assert(audios[static_cast<std::size_t>(s)]->autoplay);
}
