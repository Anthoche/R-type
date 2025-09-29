/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** entity_storage
*/

#include "Include/entity_storage.hpp"
#include "Include/entity_parser.hpp"
#include <iostream>

namespace game::storage {

static bool is_array(const nlohmann::json &data, const std::string &field) {
    return data.contains(field) && data.at(field).is_array();
}

void store_players(ecs::registry &reg, const nlohmann::json &players_data) {
    if (!is_array(players_data, "players")) {
        std::cerr << "[ERROR] Missing or invalid 'players' array in JSON." << std::endl;
        return;
    }
    for (const auto &player_data : players_data.at("players")) {
        try {
            game::parsing::parse_player(reg, player_data);
        } catch (const std::exception &e) {
            std::cerr << "[ERROR] " << e.what() << std::endl;
        }
    }
}

void store_level_entities(ecs::registry &reg, const nlohmann::json &level_data) {
    if (!level_data.contains("level") || !level_data.at("level").is_object()) {
        std::cerr << "[ERROR] Missing or invalid 'level' object in JSON." << std::endl;
        return;
    }
    const auto &level = level_data.at("level");

    // Backgrounds
    if (is_array(level, "backgrounds")) {
        for (const auto &bg_data : level.at("backgrounds")) {
            try {
                game::parsing::parse_background(reg, bg_data);
            } catch (const std::exception &e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
            }
        }
    }

    // Enemies
    if (is_array(level, "enemies")) {
        for (const auto &enemy_data : level.at("enemies")) {
            try {
                game::parsing::parse_enemy(reg, enemy_data);
            } catch (const std::exception &e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
            }
        }
    }

    // Obstacles
    if (is_array(level, "obstacles")) {
        for (const auto &obstacle_data : level.at("obstacles")) {
            try {
                game::parsing::parse_obstacle(reg, obstacle_data);
            } catch (const std::exception &e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
            }
        }
    }

    // Random elements
    if (is_array(level, "random_elements")) {
        for (const auto &element_data : level.at("random_elements")) {
            try {
                game::parsing::parse_random_element(reg, element_data);
            } catch (const std::exception &e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
            }
        }
    }

    // Sounds
    if (is_array(level, "sounds")) {
        for (const auto &sound_data : level.at("sounds")) {
            try {
                game::parsing::parse_sound(reg, sound_data);
            } catch (const std::exception &e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
            }
        }
    }

    // Texts
    if (is_array(level, "texts")) {
        for (const auto &text_data : level.at("texts")) {
            try {
                game::parsing::parse_text(reg, text_data);
            } catch (const std::exception &e) {
                std::cerr << "[ERROR] " << e.what() << std::endl;
            }
        }
    }
}

} // namespace game::storage
