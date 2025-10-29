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

/**
 * @namespace game::storage
 * @brief Contains functions for loading and storing game entities from JSON data.
 *
 * This namespace provides utilities to populate the ECS registry with entities
 * parsed from JSON configuration files, such as player data and level layouts.
 */
namespace game::storage {

    using json = nlohmann::json;

    /**
     * @brief Loads player entities from JSON data into the registry.
     *
     * Parses JSON player configuration and creates corresponding player entities
     * with their associated components in the ECS registry.
     *
     * @param reg Reference to the ECS registry where entities will be created.
     * @param players_data JSON array or object containing player configurations.
     */
    void store_players(ecs::registry &reg, const json &players_data);

    /**
     * @brief Loads level entities from JSON data into the registry.
     *
     * Parses JSON level configuration and creates all level entities including
     * enemies, obstacles, items, and other game objects in the ECS registry.
     *
     * @param reg Reference to the ECS registry where entities will be created.
     * @param level_data JSON object containing level configuration and entity definitions.
     */
    void store_level_entities(ecs::registry &reg, const json &level_data);

} // namespace game::storage