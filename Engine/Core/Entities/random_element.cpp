/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** random_element
*/

#include "Include/random_element.hpp"
#include <iostream>

namespace game::entities {

    ecs::entity_t create_random_element(ecs::registry &reg, float x, float y, float z,
        const std::string &imagePath, float width, float height, const std::string &type, float velocity) {
        auto element = reg.spawn_entity();

        float _velocity = (velocity * 100.0f) * -1.0f;
        reg.emplace_component<component::position>(element, x, y, z);
        reg.emplace_component<component::previous_position>(element, x, y, z);
        reg.emplace_component<component::velocity>(element, _velocity, 0.f);
        reg.emplace_component<component::type>(element, component::entity_type::RANDOM_ELEMENT);
        reg.emplace_component<component::collision_box>(element, width, height, 28.f);
        reg.emplace_component<component::pattern_element>(element, type);

        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = 28.f;
        draw.color = RED;
        reg.add_component<component::drawable>(element, std::move(draw));

    if (!imagePath.empty()) {
        component::sprite spr;
        spr.image_path = imagePath;
        spr.scale = 1.f;
        reg.add_component<component::sprite>(element, std::move(spr));
    }

        return element;
    }

}
