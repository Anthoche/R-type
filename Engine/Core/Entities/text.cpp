/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** text entity impl
*/

#include "Include/text.hpp"

namespace game::entities {

ecs::entity_t create_text(ecs::registry &reg, float x, float y, const std::string &content, int fontSize, const std::string &fontPath)
{
    auto e = reg.spawn_entity();
    reg.emplace_component<component::position>(e, x, y);

    component::text t;
    t.content = content;
    t.font_size = fontSize;
    reg.add_component<component::text>(e, std::move(t));

    reg.emplace_component<component::type>(e, component::entity_type::TEXT);

    if (!fontPath.empty()) {
        component::font f;
        f.font_path = fontPath;
        reg.add_component<component::font>(e, std::move(f));
    }

    return e;
}

}


