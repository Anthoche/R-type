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

void deserialize_entities(ecs::registry &reg, const nlohmann::json &j)
{
    try {
        if (!j.is_array())
            throw std::runtime_error("deserialize_entities: expected JSON array");

        for (const auto &entity_data : j) {
            if (!entity_data.contains("type")) {
                throw std::runtime_error("deserialize_entity: missing 'type' field");
            }

            int type_value = entity_data["type"];
            component::entity_type type_enum = static_cast<component::entity_type>(type_value);

            ecs::entity_t new_entity;

            switch (type_enum) {
                case component::entity_type::PLAYER:
                    new_entity = game::parsing::parse_player(reg, entity_data);
                    break;
                case component::entity_type::ENEMY:
                    new_entity = game::parsing::parse_enemy(reg, entity_data);
                    break;
                case component::entity_type::BACKGROUND:
                    new_entity = game::parsing::parse_background(reg, entity_data);
                    break;
                case component::entity_type::OBSTACLE:
                    new_entity = game::parsing::parse_obstacle(reg, entity_data);
                    break;
                case component::entity_type::RANDOM_ELEMENT:
                    new_entity = game::parsing::parse_random_element(reg, entity_data);
                    break;
                case component::entity_type::SOUND:
                    new_entity = game::parsing::parse_sound(reg, entity_data);
                    break;
                case component::entity_type::TEXT:
                    new_entity = game::parsing::parse_text(reg, entity_data);
                    break;
                default:
                    std::cerr << "[WARNING] Unknown entity type: " << type_value << std::endl;
                    continue;
            }

            std::cout << "[INFO] Deserialized entity of type " << type_value
                      << " (id=" << static_cast<std::size_t>(new_entity) << ")\n";
        }
    }
    catch (const std::exception &e) {
        std::cerr << "[ERROR] Failed to deserialize entities: " << e.what() << std::endl;
    }
}
