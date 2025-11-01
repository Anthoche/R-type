/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** player
*/

#include "Include/player.hpp"
#include "Include/hitbox.hpp"

namespace game::entities
{
    ecs::entity_t create_player(ecs::registry &reg, float x, float y,  float z, float width, float height, float depth,
        const std::string &imagePath, const std::string &modelPath,  uint32_t clientId, int health, float speed) {
        
        auto player = reg.spawn_entity();

        // ====== Position / Movement ======
        reg.emplace_component<component::position>(player, x, y, z);
        reg.emplace_component<component::previous_position>(player, x, y, z);
        reg.emplace_component<component::velocity>(player, 0.f, 0.f, 0.f);
        reg.emplace_component<component::controllable>(player, speed, true, false); // speed, can_jump, can_fly

        // ====== Stats ======
        reg.emplace_component<component::health>(player, health, health);
        reg.emplace_component<component::type>(player, component::entity_type::PLAYER);
        reg.emplace_component<component::client_id>(player, clientId);

        // ====== Visuals - USE PARSED DIMENSIONS ======
        component::drawable draw;
        draw.width = width;
        draw.height = height;
        draw.depth = depth;
        reg.add_component<component::drawable>(player, std::move(draw));

        if (!imagePath.empty()) {
            component::sprite spr;
            spr.image_path = imagePath;
            spr.scale = 1.f;
            reg.add_component<component::sprite>(player, std::move(spr));
        }

        if (!modelPath.empty()) {
            component::model3D model;
            model.model_path = modelPath;
            model.scale = 1.f;
            reg.add_component<component::model3D>(player, std::move(model));
        }

        // ====== Hitbox ======
        create_hitbox_for(reg, player);

        return player;
    }

    void setup_player_control_system(ecs::registry &reg)
    {
        reg.add_system<component::velocity, component::controllable>(
            [](ecs::registry &reg,
               ecs::sparse_array<component::velocity> &vel,
               ecs::sparse_array<component::controllable> &ctrl) 
            { });
    }

    void setup_player_bounds_system(ecs::registry &reg, float screen_width,
        float screen_height, float screen_depth) {
        reg.add_system<component::position, component::type>(
            [screen_width, screen_height, screen_depth](
                ecs::registry &reg,
                ecs::sparse_array<component::position> &pos,
                ecs::sparse_array<component::type> &type)
            {
                for (std::size_t i = 0; i < pos.size() && i < type.size(); ++i) {
                    if (pos[i] && type[i] && type[i]->value == component::entity_type::PLAYER) {
                        // X bounds
                        if (pos[i]->x < 0.f) pos[i]->x = 0.f;
                        if (pos[i]->x > screen_width - 50.f) pos[i]->x = screen_width - 50.f;
                        // Y bounds
                        if (pos[i]->y < 0.f) pos[i]->y = 0.f;
                        if (pos[i]->y > screen_height - 30.f) pos[i]->y = screen_height - 30.f;
                        // Z bounds
                        if (pos[i]->z < 0.f) pos[i]->z = 0.f;
                        if (pos[i]->z > screen_depth - 50.f) pos[i]->z = screen_depth - 50.f;
                    }
                }
            });
    }
}