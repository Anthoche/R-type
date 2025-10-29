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

/**
 * @namespace game::parsing
 * @brief Contains functions for parsing JSON data into ECS entities.
 *
 * This namespace provides utility functions to deserialize JSON-formatted game data
 * and create corresponding entities in the ECS registry with appropriate components.
 */
namespace game::parsing
{
    using json = nlohmann::json;

    /**
     * @brief Parses JSON data and creates a player entity.
     *
     * @param reg Reference to the ECS registry.
     * @param player_data JSON object containing player configuration.
     * @return The unique identifier of the created player entity.
     */
    ecs::entity_t parse_player(ecs::registry &reg, const json &player_data);

    /**
     * @brief Parses JSON data and creates a background entity.
     *
     * @param reg Reference to the ECS registry.
     * @param bg_data JSON object containing background configuration.
     * @return The unique identifier of the created background entity.
     */
    ecs::entity_t parse_background(ecs::registry &reg, const json &bg_data);

    /**
     * @brief Parses JSON data and creates an enemy entity.
     *
     * @param reg Reference to the ECS registry.
     * @param enemy_data JSON object containing enemy configuration.
     * @return The unique identifier of the created enemy entity.
     */
    ecs::entity_t parse_enemy(ecs::registry &reg, const json &enemy_data);

    /**
     * @brief Parses JSON data and creates an obstacle entity.
     *
     * @param reg Reference to the ECS registry.
     * @param obstacle_data JSON object containing obstacle configuration.
     * @return The unique identifier of the created obstacle entity.
     */
    ecs::entity_t parse_obstacle(ecs::registry &reg, const json &obstacle_data);

    /**
     * @brief Parses JSON data and creates a random element entity.
     *
     * @param reg Reference to the ECS registry.
     * @param element_data JSON object containing random element configuration.
     * @return The unique identifier of the created random element entity.
     */
    ecs::entity_t parse_random_element(ecs::registry &reg, const json &element_data);

    /**
     * @brief Parses JSON data and creates a sound entity.
     *
     * @param reg Reference to the ECS registry.
     * @param sound_data JSON object containing sound configuration.
     * @return The unique identifier of the created sound entity.
     */
    ecs::entity_t parse_sound(ecs::registry &reg, const json &sound_data);

    /**
     * @brief Parses JSON data and creates a text entity.
     *
     * @param reg Reference to the ECS registry.
     * @param text_data JSON object containing text configuration.
     */
    void parse_text(ecs::registry &reg, const json &text_data);

    /**
     * @brief Parses JSON data and creates an item entity.
     *
     * @param reg Reference to the ECS registry.
     * @param item_data JSON object containing item configuration.
     * @return The unique identifier of the created item entity.
     */
    ecs::entity_t parse_item(ecs::registry &reg, const json &item_data);

    /**
     * @brief Parses JSON data and creates a power-up entity.
     *
     * @param reg Reference to the ECS registry.
     * @param powerup_data JSON object containing power-up configuration.
     * @return The unique identifier of the created power-up entity.
     */
    ecs::entity_t parse_powerup(ecs::registry &reg, const json &powerup_data);

    /**
     * @brief Parses JSON data and creates a trap entity.
     *
     * @param reg Reference to the ECS registry.
     * @param trap_data JSON object containing trap configuration.
     * @return The unique identifier of the created trap entity.
     */
    ecs::entity_t parse_trap(ecs::registry &reg, const json &trap_data);

    /**
     * @brief Parses JSON data and creates a gate entity.
     *
     * @param reg Reference to the ECS registry.
     * @param gate_data JSON object containing gate configuration.
     * @return The unique identifier of the created gate entity.
     */
    ecs::entity_t parse_gate(ecs::registry &reg, const json &gate_data);

    /**
     * @brief Parses JSON data and creates a weapon entity.
     *
     * @param reg Reference to the ECS registry.
     * @param weapon_data JSON object containing weapon configuration.
     * @return The unique identifier of the created weapon entity.
     */
    ecs::entity_t parse_weapon(ecs::registry &reg, const json &weapon_data);

    /**
     * @brief Parses JSON data and creates a PNG image entity.
     *
     * @param reg Reference to the ECS registry.
     * @param png_data JSON object containing PNG configuration.
     * @return The unique identifier of the created PNG entity.
     */
    ecs::entity_t parse_png(ecs::registry &reg, const json &png_data);

    /**
     * @brief Parses JSON data and creates a platform entity.
     *
     * @param reg Reference to the ECS registry.
     * @param platform_data JSON object containing platform configuration.
     * @return The unique identifier of the created platform entity.
     */
    ecs::entity_t parse_platform(ecs::registry &reg, const json &platform_data);

    /**
     * @brief Parses JSON data and creates a decoration entity.
     *
     * @param reg Reference to the ECS registry.
     * @param deco_data JSON object containing decoration configuration.
     * @return The unique identifier of the created decoration entity.
     */
    ecs::entity_t parse_decoration(ecs::registry &reg, const json &deco_data);

    /**
     * @brief Parses JSON data and creates a spawner entity.
     *
     * @param reg Reference to the ECS registry.
     * @param spawner_data JSON object containing spawner configuration.
     * @return The unique identifier of the created spawner entity.
     */
    ecs::entity_t parse_spawner(ecs::registry &reg, const json &spawner_data);

    /**
     * @brief Parses JSON data and creates a checkpoint entity.
     *
     * @param reg Reference to the ECS registry.
     * @param checkpoint_data JSON object containing checkpoint configuration.
     * @return The unique identifier of the created checkpoint entity.
     */
    ecs::entity_t parse_checkpoint(ecs::registry &reg, const json &checkpoint_data);

    /**
     * @brief Parses JSON data and creates a trigger zone entity.
     *
     * @param reg Reference to the ECS registry.
     * @param zone_data JSON object containing trigger zone configuration.
     * @return The unique identifier of the created trigger zone entity.
     */
    ecs::entity_t parse_triggerzone(ecs::registry &reg, const json &zone_data);
} // namespace game::parsing