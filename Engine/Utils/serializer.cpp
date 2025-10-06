/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** serializer
*/

#include "Include/serializer.hpp"
#include "../../Core/Entities/Include/components.hpp"
#include <iostream>

namespace game::serializer {

nlohmann::json serialize_entity(ecs::registry &reg, ecs::entity_t entity) {
        nlohmann::json j;
        j["entity_id"] = static_cast<std::size_t>(entity);
        if (auto pos = reg.get_components<component::position>()[static_cast<std::size_t>(entity)]) {
            j["position"] = { {"x", pos->x}, {"y", pos->y} };
        }
        if (auto type = reg.get_components<component::type>()[static_cast<std::size_t>(entity)]) {
            j["type"] = static_cast<int>(type->value);
        } else {
            return nlohmann::json();
        }
        if (auto cid = reg.get_components<component::client_id>()[static_cast<std::size_t>(entity)]) {
            j["client_id"] = cid->id;
        }
        if (auto vel = reg.get_components<component::velocity>()[static_cast<std::size_t>(entity)]) {
            j["velocity"] = { {"vx", vel->vx}, {"vy", vel->vy} };
        }
        if (auto health = reg.get_components<component::health>()[static_cast<std::size_t>(entity)]) {
            j["health"] = { {"current", health->current}, {"max", health->max} };
        }
        if (auto sprite = reg.get_components<component::sprite>()[static_cast<std::size_t>(entity)]) {
            j["sprite"] = { {"image_path", sprite->image_path} };
        }
        if (auto drawable = reg.get_components<component::drawable>()[static_cast<std::size_t>(entity)]) {
            j["drawable"] = { 
                {"width", drawable->width}, 
                {"height", drawable->height} 
            };
        }
        if (auto cbox = reg.get_components<component::collision_box>()[static_cast<std::size_t>(entity)]) {
            j["collision_box"] = { 
                {"width", cbox->width}, 
                {"height", cbox->height} 
            };
        }
        return j;
    }
}