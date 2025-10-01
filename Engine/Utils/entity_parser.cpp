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
    static void check_field(
        const nlohmann::json &data,
        const std::string &field,
        nlohmann::json::value_t expected_type)
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

    ecs::entity_t parse_player(ecs::registry &reg, const nlohmann::json &player_data)
    {
        try {
            check_field(player_data, "x", nlohmann::json::value_t::number_float);
            check_field(player_data, "y", nlohmann::json::value_t::number_float);
            check_field(player_data, "client_id", nlohmann::json::value_t::number_unsigned);

            float x = player_data.at("x");
            float y = player_data.at("y");
            uint32_t clientId = player_data.at("client_id");
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

    ecs::entity_t parse_background(ecs::registry &reg, const nlohmann::json &bg_data)
    {
        try {
            check_field(bg_data, "x", nlohmann::json::value_t::number_float);
            check_field(bg_data, "y", nlohmann::json::value_t::number_float);
            check_field(bg_data, "width", nlohmann::json::value_t::number_float);
            check_field(bg_data, "height", nlohmann::json::value_t::number_float);

            float x = bg_data.at("x");
            float y = bg_data.at("y");
            float width = bg_data.at("width");
            float height = bg_data.at("height");
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

    ecs::entity_t parse_enemy(ecs::registry &reg, const nlohmann::json &enemy_data)
    {
        try {
            check_field(enemy_data, "x", nlohmann::json::value_t::number_float);
            check_field(enemy_data, "y", nlohmann::json::value_t::number_float);

            float x = enemy_data.at("x");
            float y = enemy_data.at("y");
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
            check_field(obstacle_data, "x", nlohmann::json::value_t::number_float);
            check_field(obstacle_data, "y", nlohmann::json::value_t::number_float);

            float x = obstacle_data.at("x");
            float y = obstacle_data.at("y");
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
            check_field(element_data, "x", nlohmann::json::value_t::number_float);
            check_field(element_data, "y", nlohmann::json::value_t::number_float);
            check_field(element_data, "width", nlohmann::json::value_t::number_float);
            check_field(element_data, "height", nlohmann::json::value_t::number_float);

            float x = element_data.at("x");
            float y = element_data.at("y");
            float width = element_data.at("width");
            float height = element_data.at("height");
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
            check_field(sound_data, "sound_path", nlohmann::json::value_t::string);

            std::string sound_path = sound_data.at("sound_path");

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

    ecs::entity_t parse_text(ecs::registry &reg, const nlohmann::json &text_data)
    {
        try {
            check_field(text_data, "x", nlohmann::json::value_t::number_float);
            check_field(text_data, "y", nlohmann::json::value_t::number_float);
            check_field(text_data, "content", nlohmann::json::value_t::string);
            check_field(text_data, "font_size", nlohmann::json::value_t::number_integer);

            float x = text_data.at("x");
            float y = text_data.at("y");
            std::string content = text_data.at("content");
            int font_size = text_data.at("font_size");
            std::string font_path = text_data.value("font_path", "");

            if (!font_path.empty() && !std::ifstream(font_path).good()) {
                std::cerr << "[WARNING] Font file not found: " << font_path << std::endl;
            }

            std::cout << "[DEBUG] Parsed text: '" << content << "' at (" << x << ", " << y << ")" << std::endl;

            //return game::entities::create_text(reg, x, y, content, font_size, font_path);;
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse text: ") + e.what());
        }
    }
} // namespace game::parsing
