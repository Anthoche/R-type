/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** random element entity impl
*/

#include "Include/random_element.hpp"

namespace game::entities {

ecs::entity_t create_random_element(ecs::registry &reg, float x, float y, float width, float height, const std::string &imagePath, const std::string &soundPath, float volume, bool loop, bool autoplay)
{
    auto e = reg.spawn_entity();
    reg.emplace_component<component::position>(e, x, y);

    component::drawable drw;
    drw.width = width;
    drw.height = height;
    reg.add_component<component::drawable>(e, std::move(drw));

    if (!imagePath.empty()) {
        component::sprite spr;
        spr.image_path = imagePath;
        reg.add_component<component::sprite>(e, std::move(spr));
    }

    if (!soundPath.empty()) {
        component::audio au;
        au.sound_path = soundPath;
        au.volume = volume;
        au.loop = loop;
        au.autoplay = autoplay;
        reg.add_component<component::audio>(e, std::move(au));
    }

    return e;
}

}


