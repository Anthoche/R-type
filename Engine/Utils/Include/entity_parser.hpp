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

namespace game::parsing
{
    using json = nlohmann::json;

    ecs::entity_t parse_player(ecs::registry &reg, const json &player_data);
    ecs::entity_t parse_background(ecs::registry &reg, const json &bg_data);
    ecs::entity_t parse_enemy(ecs::registry &reg, const json &enemy_data);
    ecs::entity_t parse_obstacle(ecs::registry &reg, const json &obstacle_data);
    ecs::entity_t parse_random_element(ecs::registry &reg, const json &element_data);
    ecs::entity_t parse_sound(ecs::registry &reg, const json &sound_data);
    void parse_text(ecs::registry &reg, const json &text_data);

    ecs::entity_t parse_item(ecs::registry &reg, const json &item_data);
    ecs::entity_t parse_powerup(ecs::registry &reg, const json &powerup_data);
    ecs::entity_t parse_trap(ecs::registry &reg, const json &trap_data);
    ecs::entity_t parse_gate(ecs::registry &reg, const json &gate_data);
    ecs::entity_t parse_weapon(ecs::registry &reg, const json &weapon_data);
    ecs::entity_t parse_png(ecs::registry &reg, const json &png_data);
    ecs::entity_t parse_platform(ecs::registry &reg, const json &platform_data);
    ecs::entity_t parse_decoration(ecs::registry &reg, const json &deco_data);
    ecs::entity_t parse_spawner(ecs::registry &reg, const json &spawner_data);
    ecs::entity_t parse_checkpoint(ecs::registry &reg, const json &checkpoint_data);
    ecs::entity_t parse_triggerzone(ecs::registry &reg, const json &zone_data);
} // namespace game::parsing
