/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** random_element
*/

#include "Include/random_element.hpp"

namespace game::entities {

    ecs::entity_t create_random_element(ecs::registry &reg, float x, float y, float z,
        float width, float height, float depth, const std::string &imagePath,
        const std::string &soundPath, float volume, bool loop, bool autoplay)
    {
        auto e = reg.spawn_entity();
        reg.emplace_component<component::position>(e, x, y, z);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        reg.add_component<component::drawable>(e, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
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

        reg.emplace_component<component::type>(e, component::entity_type::RANDOM_ELEMENT);
        return e;
    }

}
