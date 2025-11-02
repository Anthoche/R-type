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

        if (is_array(level, "backgrounds")) {
            for (const auto &bg_data : level.at("backgrounds")) {
                try { game::parsing::parse_background(reg, bg_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "enemies")) {
            for (const auto &enemy_data : level.at("enemies")) {
                try { game::parsing::parse_enemy(reg, enemy_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "obstacles")) {
            for (const auto &obstacle_data : level.at("obstacles")) {
                try { game::parsing::parse_obstacle(reg, obstacle_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "random_elements")) {
            for (const auto &element_data : level.at("random_elements")) {
                try { game::parsing::parse_random_element(reg, element_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "sounds")) {
            for (const auto &sound_data : level.at("sounds")) {
                try { game::parsing::parse_sound(reg, sound_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "texts")) {
            for (const auto &text_data : level.at("texts")) {
                try { game::parsing::parse_text(reg, text_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "items")) {
            for (const auto &item_data : level.at("items")) {
                try { game::parsing::parse_item(reg, item_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "powerups")) {
            for (const auto &powerup_data : level.at("powerups")) {
                try { game::parsing::parse_powerup(reg, powerup_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "traps")) {
            for (const auto &trap_data : level.at("traps")) {
                try { game::parsing::parse_trap(reg, trap_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "gates")) {
            for (const auto &gate_data : level.at("gates")) {
                try { game::parsing::parse_gate(reg, gate_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "weapons")) {
            for (const auto &weapon_data : level.at("weapons")) {
                try { game::parsing::parse_weapon(reg, weapon_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "platforms")) {
            for (const auto &platform_data : level.at("platforms")) {
                try { game::parsing::parse_platform(reg, platform_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "decorations")) {
            for (const auto &decor_data : level.at("decorations")) {
                try { game::parsing::parse_decoration(reg, decor_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "spawners")) {
            for (const auto &spawner_data : level.at("spawners")) {
                try { game::parsing::parse_spawner(reg, spawner_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "checkpoints")) {
            for (const auto &checkpoint_data : level.at("checkpoints")) {
                try { game::parsing::parse_checkpoint(reg, checkpoint_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }

        if (is_array(level, "triggerzones")) {
            for (const auto &zone_data : level.at("triggerzones")) {
                try { game::parsing::parse_triggerzone(reg, zone_data); }
                catch (const std::exception &e) { std::cerr << "[ERROR] " << e.what() << std::endl; }
            }
        }
    }

} // namespace game::storage
