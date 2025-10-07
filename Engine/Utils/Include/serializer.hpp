/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** serializer
*/

#pragma once

#include <nlohmann/json.hpp>
#include "../../Core/Include/registry.hpp"
#include "../../Core/Entities/Include/components.hpp"

namespace game::serializer {

    nlohmann::json serialize_entity(ecs::registry &reg, ecs::entity_t entity);

    void deserialize_entities(ecs::registry &reg, const nlohmann::json &j)

} // namespace game::net::serializer