/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sound entity impl
*/

#include "Include/sound.hpp"

namespace game::entities {

ecs::entity_t create_sound(ecs::registry &reg, const std::string &soundPath, float volume, bool loop, bool autoplay)
{
    auto e = reg.spawn_entity();
    reg.emplace_component<component::type>(e, component::entity_type::SOUND);

    component::audio au;
    au.sound_path = soundPath;
    au.volume = volume;
    au.loop = loop;
    au.autoplay = autoplay;
    reg.add_component<component::audio>(e, std::move(au));

    return e;
}

}


