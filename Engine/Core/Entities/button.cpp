/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** button
*/

#include "Include/button.hpp"
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    ecs::entity_t create_button(ecs::registry &reg, const std::string &id, const std::string &displayName,
        float x, float y, float z, float width, float height, Color color, Color textColor, int fontSize) {
        auto button = reg.spawn_entity();

        // ====== Position ======
        reg.emplace_component<component::position>(button, x, y, z);

        // ====== Interaction ======
        reg.emplace_component<component::clickable>(button, id);
        reg.emplace_component<component::hoverable>(button, id);

        // ====== Type ======
        reg.emplace_component<component::type>(button, component::entity_type::BUTTON);

        // ====== Drawable ======
        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = 5.f; // slight depth for UI 3D effect
        draw.color = color;
        reg.add_component<component::drawable>(button, std::move(draw));

        // ====== Text ======
        component::text text;
        text.spacing = -1.f;
        text.font_size = fontSize;
        text.content = displayName;
        text.color = textColor;
        reg.emplace_component<component::text>(button, std::move(text));

        return button;
    }

}
