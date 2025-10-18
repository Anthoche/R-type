/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** entity_parser
*/

#include "Include/entity_parser.hpp"
#include "../Core/Entities/Include/player.hpp"
#include "../Core/Entities/Include/background.hpp"
#include "../Core/Entities/Include/enemy.hpp"
#include "../Core/Entities/Include/obstacle.hpp"
#include "../Core/Entities/Include/random_element.hpp"
#include "../Core/Entities/Include/sound.hpp"
#include "../Core/Entities/Include/text.hpp"
#include <fstream>
#include <stdexcept>

namespace game::parsing
{
    static void check_field(const nlohmann::json &data, const std::string &field, nlohmann::json::value_t expected_type)
    {
        if (!data.contains(field)) {
            throw std::runtime_error("Missing field: " + field);
        }
        if (!data.at(field).is_number() &&
            !data.at(field).is_string() &&
            !data.at(field).is_boolean())
        {
            if (data.at(field).type() != expected_type) {
                throw std::runtime_error("Invalid type for field: " + field);
            }
        }
    }

    // Helper to get float from JSON (handles both direct and nested formats)
    static float get_float_value(const nlohmann::json &data, const std::string &field, float default_val = 0.0f)
    {
        if (data.contains(field)) {
            return data.at(field).get<float>();
        }
        return default_val;
    }
    
    ecs::entity_t parse_background(ecs::registry &reg, const nlohmann::json &bg_data)
    {
        try {
            // Support both formats
            float x, y;
            if (bg_data.contains("position") && bg_data["position"].is_object()) {
                x = bg_data["position"]["x"];
                y = bg_data["position"]["y"];
            } else {
                check_field(bg_data, "x", nlohmann::json::value_t::number_float);
                check_field(bg_data, "y", nlohmann::json::value_t::number_float);
                x = bg_data.at("x");
                y = bg_data.at("y");
            }

            // Width and height with defaults
            float width = get_float_value(bg_data, "width", 1200.0f);
            float height = get_float_value(bg_data, "height", 200.0f);
            
            std::string image_path = bg_data.value("image_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Background image file not found: " << image_path << std::endl;
            }

            float scale = bg_data.value("scale", 1.0f);

            std::cout << "[DEBUG] Parsed background at (" << x << ", " << y << ")" << std::endl;

            return game::entities::create_background(reg, x, y, width, height, image_path, scale);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse background: ") + e.what());
        }
    }

    ecs::entity_t parse_player(ecs::registry &reg, const nlohmann::json &player_data)
    {
        try {
            // Support both formats: direct x/y and nested position
            float x, y;
            if (player_data.contains("position") && player_data["position"].is_object()) {
                x = player_data["position"]["x"];
                y = player_data["position"]["y"];
            } else {
                check_field(player_data, "x", nlohmann::json::value_t::number_float);
                check_field(player_data, "y", nlohmann::json::value_t::number_float);
                x = player_data.at("x");
                y = player_data.at("y");
            }

            uint32_t clientId = 0;
            if (player_data.contains("client_id")) {
                clientId = player_data.at("client_id");
            }

            std::string image_path = player_data.value("image_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Player image file not found: " << image_path << std::endl;
            }

            std::cout << "[DEBUG] Parsed player (client_id=" << clientId
                      << ") at (" << x << ", " << y << ")" << std::endl;

            return game::entities::create_player(reg, x, y, image_path, clientId);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse player: ") + e.what());
        }
    }


    ecs::entity_t parse_enemy(ecs::registry &reg, const nlohmann::json &enemy_data)
    {
        try {
            // Support both formats
            float x, y;
            if (enemy_data.contains("position") && enemy_data["position"].is_object()) {
                x = enemy_data["position"]["x"];
                y = enemy_data["position"]["y"];
            } else {
                check_field(enemy_data, "x", nlohmann::json::value_t::number_float);
                check_field(enemy_data, "y", nlohmann::json::value_t::number_float);
                x = enemy_data.at("x");
                y = enemy_data.at("y");
            }

            std::string image_path = enemy_data.value("image_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Enemy image file not found: " << image_path << std::endl;
            }

            std::cout << "[DEBUG] Parsed enemy at (" << x << ", " << y << ")" << std::endl;

            return game::entities::create_enemy(reg, x, y, image_path);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse enemy: ") + e.what());
        }
    }

    ecs::entity_t parse_obstacle(ecs::registry &reg, const nlohmann::json &obstacle_data)
    {
        try {
            // Support both formats
            float x, y;
            if (obstacle_data.contains("position") && obstacle_data["position"].is_object()) {
                x = obstacle_data["position"]["x"];
                y = obstacle_data["position"]["y"];
            } else {
                check_field(obstacle_data, "x", nlohmann::json::value_t::number_float);
                check_field(obstacle_data, "y", nlohmann::json::value_t::number_float);
                x = obstacle_data.at("x");
                y = obstacle_data.at("y");
            }

            std::string image_path = obstacle_data.value("image_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Obstacle image file not found: " << image_path << std::endl;
            }

            std::cout << "[DEBUG] Parsed obstacle at (" << x << ", " << y << ")" << std::endl;

            return game::entities::create_obstacle(reg, x, y, image_path);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse obstacle: ") + e.what());
        }
    }

    ecs::entity_t parse_random_element(ecs::registry &reg, const nlohmann::json &element_data)
    {
        try {
            // Support both formats
            float x, y;
            if (element_data.contains("position") && element_data["position"].is_object()) {
                x = element_data["position"]["x"];
                y = element_data["position"]["y"];
            } else {
                check_field(element_data, "x", nlohmann::json::value_t::number_float);
                check_field(element_data, "y", nlohmann::json::value_t::number_float);
                x = element_data.at("x");
                y = element_data.at("y");
            }

            float width = get_float_value(element_data, "width", 100.0f);
            float height = get_float_value(element_data, "height", 100.0f);
            
            std::string image_path = element_data.value("image_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Random element image file not found: " << image_path << std::endl;
            }

            std::string sound_path = element_data.value("sound_path", "");

            if (!sound_path.empty() && !std::ifstream(sound_path).good()) {
                std::cerr << "[WARNING] Random element sound file not found: " << sound_path << std::endl;
            }

            float volume = element_data.value("volume", 1.0f);
            bool loop = element_data.value("loop", false);
            bool autoplay = element_data.value("autoplay", false);

            std::cout << "[DEBUG] Parsed random element at (" << x << ", " << y << ")" << std::endl;

            return game::entities::create_random_element(reg, x, y, width, height, image_path, sound_path, volume, loop, autoplay);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse random element: ") + e.what());
        }
    }

    ecs::entity_t parse_sound(ecs::registry &reg, const nlohmann::json &sound_data)
    {
        try {
            std::string sound_path = sound_data.value("sound_path", "");
            
            if (sound_path.empty()) {
                throw std::runtime_error("Missing sound_path field");
            }

            if (!std::ifstream(sound_path).good()) {
                std::cerr << "[WARNING] Sound file not found: " << sound_path << std::endl;
            }

            float volume = sound_data.value("volume", 1.0f);
            bool loop = sound_data.value("loop", false);
            bool autoplay = sound_data.value("autoplay", false);

            std::cout << "[DEBUG] Parsed sound: " << sound_path << std::endl;

            return game::entities::create_sound(reg, sound_path, volume, loop, autoplay);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse sound: ") + e.what());
        }
    }

    void parse_text(ecs::registry &reg, const nlohmann::json &text_data)
    {
        try {
            // Support both formats
            float x, y;
            if (text_data.contains("position") && text_data["position"].is_object()) {
                x = text_data["position"]["x"];
                y = text_data["position"]["y"];
            } else {
                check_field(text_data, "x", nlohmann::json::value_t::number_float);
                check_field(text_data, "y", nlohmann::json::value_t::number_float);
                x = text_data.at("x");
                y = text_data.at("y");
            }

            std::string content = text_data.value("content", "");
            int font_size = text_data.value("font_size", 12);
            std::string font_path = text_data.value("font_path", "");

            if (!font_path.empty() && !std::ifstream(font_path).good()) {
                std::cerr << "[WARNING] Font file not found: " << font_path << std::endl;
            }

            std::cout << "[DEBUG] Parsed text: '" << content << "' at (" << x << ", " << y << ")" << std::endl;
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse text: ") + e.what());
        }
    }

    ecs::entity_t parse_item(ecs::registry &reg, const json &item_data)
    {
        try {
            float x = item_data.value("x", 0.f);
            float y = item_data.value("y", 0.f);
            float z = item_data.value("z", 0.f);
            std::string image_path = item_data.value("image_path", "");
            return game::entities::create_item(reg, x, y, z, image_path);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse item: ") + e.what());
        }
    }

    ecs::entity_t parse_powerup(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string image_path = data.value("image_path", "");
            std::string effect = data.value("effect", "none");
            float value = data.value("value", 0.f);
            return game::entities::create_powerup(reg, x, y, z, image_path, effect, value);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse powerup: ") + e.what());
        }
    }

    ecs::entity_t parse_trap(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string image_path = data.value("image_path", "");
            float damage = data.value("damage", 10.f);
            return game::entities::create_trap(reg, x, y, z, image_path, damage);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse trap: ") + e.what());
        }
    }

    ecs::entity_t parse_gate(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string image_path = data.value("image_path", "");
            bool locked = data.value("locked", false);
            std::string target_scene = data.value("target_scene", "");
            return game::entities::create_gate(reg, x, y, z, image_path, locked, target_scene);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse gate: ") + e.what());
        }
    }

    ecs::entity_t parse_weapon(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string image_path = data.value("image_path", "");
            float damage = data.value("damage", 20.f);
            float cooldown = data.value("cooldown", 1.0f);
            return game::entities::create_weapon(reg, x, y, z, image_path, damage, cooldown);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse weapon: ") + e.what());
        }
    }

    ecs::entity_t parse_png(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string image_path = data.value("image_path", "");
            return game::entities::create_png(reg, x, y, z, image_path);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse PNG: ") + e.what());
        }
    }

    ecs::entity_t parse_platform(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            float width = data.value("width", 200.f);
            float height = data.value("height", 40.f);
            std::string image_path = data.value("image_path", "");
            return game::entities::create_platform(reg, x, y, z, width, height, image_path);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse platform: ") + e.what());
        }
    }

    ecs::entity_t parse_decoration(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string image_path = data.value("image_path", "");
            return game::entities::create_decoration(reg, x, y, z, image_path);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse decoration: ") + e.what());
        }
    }

    ecs::entity_t parse_spawner(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string spawn_type = data.value("spawn_type", "enemy");
            float interval = data.value("interval", 3.f);
            return game::entities::create_spawner(reg, x, y, z, spawn_type, interval);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse spawner: ") + e.what());
        }
    }

    ecs::entity_t parse_checkpoint(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            std::string id = data.value("id", "checkpoint");
            return game::entities::create_checkpoint(reg, x, y, z, id);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse checkpoint: ") + e.what());
        }
    }

    ecs::entity_t parse_triggerzone(ecs::registry &reg, const json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);
            float width = data.value("width", 100.f);
            float height = data.value("height", 100.f);
            std::string event = data.value("event", "");
            return game::entities::create_triggerzone(reg, x, y, z, width, height, event);
        } catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse trigger zone: ") + e.what());
        }
    }

} // namespace game::parsing