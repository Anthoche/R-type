/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** entity_parser
*/

#pragma once

#include <nlohmann/json.hpp>
#include "../Engine/Core/Include/registry.hpp"
#include "components.hpp"
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
