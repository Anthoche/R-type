/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** background entity impl
*/

#include "Include/background.hpp"

namespace game::entities {

ecs::entity_t create_background(ecs::registry &reg, float x, float y, float width, float height, const std::string &imagePath, float scale)
{
    auto e = reg.spawn_entity();
    reg.emplace_component<component::position>(e, x, y);
    component::drawable drw;
    drw.width = width;
    drw.height = height;
    reg.add_component<component::drawable>(e, std::move(drw));

    reg.emplace_component<component::type>(e, component::entity_type::BACKGROUND);

    if (!imagePath.empty()) {
        component::sprite spr;
        spr.image_path = imagePath;
        spr.scale = scale;
        reg.add_component<component::sprite>(e, std::move(spr));
    }

    return e;
}

}


