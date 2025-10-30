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
#include "../Core/Entities/Include/item.hpp"
#include "../Core/Entities/Include/powerup.hpp"
#include "../Core/Entities/Include/trap.hpp"
#include "../Core/Entities/Include/gate.hpp"
#include "../Core/Entities/Include/weapon.hpp"
#include "../Core/Entities/Include/platform.hpp"
#include "../Core/Entities/Include/decoration.hpp"
#include "../Core/Entities/Include/spawner.hpp"
#include "../Core/Entities/Include/checkpoint.hpp"
#include "../Core/Entities/Include/triggerzone.hpp"
#include <fstream>
#include <stdexcept>
#include <initializer_list>

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

    static float read_dimension(const nlohmann::json &data,
                                std::initializer_list<const char*> keys,
                                float default_val)
    {
        for (const auto *key : keys) {
            auto it = data.find(key);
            if (it != data.end() && (it->is_number_float() || it->is_number_integer() || it->is_number_unsigned())) {
                return it->get<float>();
            }
        }
        return default_val;
    }
    
    ecs::entity_t parse_background(ecs::registry &reg, const nlohmann::json &bg_data)
    {
        try {
            float x = 0.0f, y = 0.0f, z = 0.0f;

            if (bg_data.contains("position") && bg_data["position"].is_object()) {
                const auto &pos = bg_data["position"];
                x = pos.value("x", 0.0f);
                y = pos.value("y", 0.0f);
                z = pos.value("z", 0.0f);
            } else {
                x = bg_data.value("x", 0.0f);
                y = bg_data.value("y", 0.0f);
                z = bg_data.value("z", 0.0f);
            }

            float width  = bg_data.value("width", 1200.0f);
            float height = bg_data.value("height", 200.0f);
            float depth  = bg_data.value("depth", 1.0f);

            std::string image_path = bg_data.value("image_path", "");
            std::string model_path = bg_data.value("model_path", "");
            float scale = bg_data.value("scale", 1.0f);

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Background image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Background model file not found: " << model_path << std::endl;
            }
            return game::entities::create_background(reg, x, y, z, width, height, depth, image_path, model_path, scale);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse background: ") + e.what());
        }
    }

    ecs::entity_t parse_player(ecs::registry &reg, const nlohmann::json &player_data)
    {
        try {
            float x = 0.0f, y = 0.0f, z = 0.0f;

            if (player_data.contains("position") && player_data["position"].is_object()) {
                const auto &pos = player_data["position"];
                x = pos.value("x", 0.0f);
                y = pos.value("y", 0.0f);
                z = pos.value("z", 0.0f);
            } else {
                x = player_data.value("x", 0.0f);
                y = player_data.value("y", 0.0f);
                z = player_data.value("z", 0.0f);
            }

            float width = read_dimension(player_data, {"width", "w"}, 33.0f);
            float height = read_dimension(player_data, {"height", "h"}, 16.0f);
            float depth = read_dimension(player_data, {"depth", "d"}, 16.0f);

            uint32_t clientId = player_data.value("client_id", 0u);
            int health = player_data.value("health", 100);
            float speed = player_data.value("speed", 300.0f);

            std::string image_path = player_data.value("image_path", "");
            std::string model_path = player_data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Player image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Player model file not found: " << model_path << std::endl;
            }

            return game::entities::create_player(reg, x, y, z, width, height, depth, image_path, model_path,  clientId, health, speed);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse player: ") + e.what());
        }
    }


    ecs::entity_t parse_enemy(ecs::registry &reg, const nlohmann::json &enemy_data)
    {
        try {
            float x = 0.0f, y = 0.0f, z = 0.0f, velocity = 0.0f, width = 0, height = 0;
            int health = 0;

            if (enemy_data.contains("position") && enemy_data["position"].is_object()) {
                const auto &pos = enemy_data["position"];
                x = pos.value("x", 0.0f);
                y = pos.value("y", 0.0f);
                z = pos.value("z", 0.0f);
            } else {
                x = enemy_data.value("x", 0.0f);
                y = enemy_data.value("y", 0.0f);
                z = enemy_data.value("z", 0.0f);
            }

            health = enemy_data.value("health", 0);
            velocity = enemy_data.value("speed", 0.0f);
            width = enemy_data.value("w", 0.0f);
            height = enemy_data.value("h", 0.0f);
            std::string image_path = enemy_data.value("image_path", "");
            std::string model_path = enemy_data.value("model_path", "");
            std::string pattern = enemy_data.value("pattern", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Enemy image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Enemy model file not found: " << model_path << std::endl;
            }
            return game::entities::create_enemy(reg, x, y, z, image_path, width, height, model_path, pattern, health, velocity);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse enemy: ") + e.what());
        }
    }

    ecs::entity_t parse_obstacle(ecs::registry &reg, const nlohmann::json &obstacle_data)
    {
        try {
            float x = 0.0f, y = 0.0f, z = 0.0f, velocity = 0.0f, width = 0, height = 0;;

            if (obstacle_data.contains("position") && obstacle_data["position"].is_object()) {
                const auto &pos = obstacle_data["position"];
                x = pos.value("x", 0.0f);
                y = pos.value("y", 0.0f);
                z = pos.value("z", 0.0f);
            } else {
                x = obstacle_data.value("x", 0.0f);
                y = obstacle_data.value("y", 0.0f);
                z = obstacle_data.value("z", 0.0f);
            }

            std::string image_path = obstacle_data.value("image_path", "");
            std::string model_path = obstacle_data.value("model_path", "");
            velocity = obstacle_data.value("speed", 0.0f);
            width = read_dimension(obstacle_data, {"width", "w"}, 0.0f);
            height = read_dimension(obstacle_data, {"height", "h"}, 0.0f);

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Obstacle image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Obstacle model file not found: " << model_path << std::endl;
            }
            return game::entities::create_obstacle(reg, x, y, z, image_path, model_path, velocity, width, height);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse obstacle: ") + e.what());
        }
    }

    ecs::entity_t parse_random_element(ecs::registry &reg, const nlohmann::json &element_data)
    {
                try {
            float x = 0.0f, y = 0.0f, z = 0.0f, velocity = 0.0f, width = 0, height = 0;
            if (element_data.contains("position") && element_data["position"].is_object()) {
                const auto &pos = element_data["position"];
                x = pos.value("x", 0.0f);
                y = pos.value("y", 0.0f);
                z = pos.value("z", 0.0f);
            } else {
                x = element_data.value("x", 0.0f);
                y = element_data.value("y", 0.0f);
                z = element_data.value("z", 0.0f);
            }

            velocity = element_data.value("speed", 0.0f);
            width = element_data.value("w", 0.0f);
            height = element_data.value("h", 0.0f);
            std::string image_path = element_data.value("image_path", "");
            std::string type = element_data.value("type", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] element image file not found: " << image_path << std::endl;
            }
            return game::entities::create_random_element(reg, x, y, z, image_path, width, height, type, velocity);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse element: ") + e.what());
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
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse text: ") + e.what());
        }
    }

    ecs::entity_t parse_item(ecs::registry &reg, const nlohmann::json &item_data)
    {
        try {
            float x = item_data.value("x", 0.f);
            float y = item_data.value("y", 0.f);
            float z = item_data.value("z", 0.f);

            float width  = item_data.value("width", 32.f);
            float height = item_data.value("height", 32.f);
            float depth  = item_data.value("depth", 32.f);

            std::string image_path = item_data.value("image_path", "");
            std::string model_path = item_data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Item image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Item model file not found: " << model_path << std::endl;
            }
            return game::entities::create_item(
                reg, x, y, z, width, height, depth, image_path, model_path
            );
        } 
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse item: ") + e.what());
        }
    }

    ecs::entity_t parse_powerup(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 32.f);
            float height = data.value("height", 32.f);
            float depth  = data.value("depth", 32.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Powerup image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Powerup model file not found: " << model_path << std::endl;
            }
            return game::entities::create_powerup(reg, x, y, z, width, height, depth, image_path, model_path);
        } 
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse powerup: ") + e.what());
        }
    }

    ecs::entity_t parse_trap(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 32.f);
            float height = data.value("height", 32.f);
            float depth  = data.value("depth", 32.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Trap image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Trap model file not found: " << model_path << std::endl;
            }
            return game::entities::create_trap(reg, x, y, z, width, height, depth, image_path, model_path);
        } 
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse trap: ") + e.what());
        }
    }

    ecs::entity_t parse_gate(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 64.f);
            float height = data.value("height", 128.f);
            float depth  = data.value("depth", 32.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Gate image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Gate model file not found: " << model_path << std::endl;
            }
            return game::entities::create_gate(reg, x, y, z, width, height, depth, image_path, model_path);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse gate: ") + e.what());
        }
    }

    ecs::entity_t parse_weapon(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 32.f);
            float height = data.value("height", 32.f);
            float depth  = data.value("depth", 32.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Weapon image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Weapon model file not found: " << model_path << std::endl;
            }
            return game::entities::create_weapon(reg, x, y, z, width, height, depth, image_path, model_path);
        } 
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse weapon: ") + e.what());
        }
    }

    ecs::entity_t parse_png(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 100.f);
            float height = data.value("height", 100.f);
            float depth  = data.value("depth", 10.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] PNG image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] PNG model file not found: " << model_path << std::endl;
            }
            //return game::entities::create_png(reg, x, y, z, width, height, depth, image_path, model_path);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse PNG: ") + e.what());
        }
    }

    ecs::entity_t parse_platform(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = read_dimension(data, {"width", "w"}, 200.f);
            float height = read_dimension(data, {"height", "h"}, 40.f);
            float depth  = read_dimension(data, {"depth", "d"}, 20.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Platform image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Platform model file not found: " << model_path << std::endl;
            }
            return game::entities::create_platform(
                reg, x, y, z, width, height, depth, image_path, model_path
            );
        } 
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse platform: ") + e.what());
        }
    }

    ecs::entity_t parse_decoration(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 50.f);
            float height = data.value("height", 50.f);
            float depth  = data.value("depth", 50.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Decoration image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Decoration model file not found: " << model_path << std::endl;
            }
            return game::entities::create_decoration(reg, x, y, z, width, height, depth, image_path, model_path);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse decoration: ") + e.what());
        }
    }

    ecs::entity_t parse_spawner(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 50.f);
            float height = data.value("height", 50.f);
            float depth  = data.value("depth", 50.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Spawner image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Spawner model file not found: " << model_path << std::endl;
            }
            return game::entities::create_spawner(
                reg, x, y, z, width, height, depth, image_path, model_path
            );
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse spawner: ") + e.what());
        }
    }

    ecs::entity_t parse_checkpoint(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 50.f);
            float height = data.value("height", 50.f);
            float depth  = data.value("depth", 50.f);

            std::string image_path = data.value("image_path", "");
            std::string model_path = data.value("model_path", "");

            if (!image_path.empty() && !std::ifstream(image_path).good()) {
                std::cerr << "[WARNING] Checkpoint image file not found: " << image_path << std::endl;
            }
            if (!model_path.empty() && !std::ifstream(model_path).good()) {
                std::cerr << "[WARNING] Checkpoint model file not found: " << model_path << std::endl;
            }
            return game::entities::create_checkpoint(reg, x, y, z, width, height, depth, image_path, model_path);
        }
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse checkpoint: ") + e.what());
        }
    }

    ecs::entity_t parse_triggerzone(ecs::registry &reg, const nlohmann::json &data)
    {
        try {
            float x = data.value("x", 0.f);
            float y = data.value("y", 0.f);
            float z = data.value("z", 0.f);

            float width  = data.value("width", 100.f);
            float height = data.value("height", 100.f);
            float depth  = data.value("depth", 50.f);

            std::string event_id = data.value("event", "");

            return game::entities::create_triggerzone(reg, x, y, z, width, height, depth, event_id);
        } 
        catch (const std::exception &e) {
            throw std::runtime_error(std::string("Failed to parse trigger zone: ") + e.what());
        }
    }

} // namespace game::parsing
