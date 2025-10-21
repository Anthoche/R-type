/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TriggerZone
*/

#pragma once
#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

ecs::entity_t create_triggerzone(ecs::registry &reg,float x, float y, float z = 0.f,
    float width = 100.f, float height = 100.f, float depth = 100.f, const std::string &id = "");

}
