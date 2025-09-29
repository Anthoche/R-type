/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** entity_storage
*/

#pragma once


#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>
#include "../../Core/Include/registry.hpp"



namespace game::storage {

using json = nlohmann::json;

void store_players(ecs::registry &reg, const json &players_data);
void store_level_entities(ecs::registry &reg, const json &level_data);

} // namespace game::storage
