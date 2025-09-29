/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** entity_parser
*/

#pragma once

#include "../../Core/Include/registry.hpp"
#include "../../Core/Entities/Include/components.hpp"
#include <nlohmann/json.hpp>
#include <iostream>


namespace game::parsing {

using json = nlohmann::json;

ecs::entity_t parse_player(ecs::registry &reg, const json &player_data);
ecs::entity_t parse_background(ecs::registry &reg, const json &bg_data);
ecs::entity_t parse_enemy(ecs::registry &reg, const json &enemy_data);
ecs::entity_t parse_obstacle(ecs::registry &reg, const json &obstacle_data);
ecs::entity_t parse_random_element(ecs::registry &reg, const json &element_data);
ecs::entity_t parse_sound(ecs::registry &reg, const json &sound_data);
ecs::entity_t parse_text(ecs::registry &reg, const json &text_data);

} // namespace game::parsing
