/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameScene
*/
#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include "../../Engine/Rendering/scene/AScene.hpp"
#include "../../Engine/Core/Include/entity.hpp"
#include "../../Engine/Core/Entities/Include/player.hpp"
#include "../../Engine/Core/Entities/Include/enemy.hpp"
#include "../../Engine/Core/Entities/Include/obstacle.hpp"
#include "../../Engine/Core/Entities/Include/hitbox.hpp"
#include "../../Engine/Core/Entities/Include/background.hpp"
#include "../../Engine/Core/Entities/Include/sound.hpp"
#include "../../Engine/Core/Entities/Include/text.hpp"
#include "../../Engine/Core/Entities/Include/random_element.hpp"
#include "../../Physics/Include/Collision.hpp"
#include "../Game.hpp"
#include <unordered_map>

namespace game::scene {

    class GameScene : public AScene {
        public:
            GameScene(Game &game);
            ~GameScene() override = default;

            void init() override;
            void render() override;
            void handleEvents() override;
            void onClose() override;

            void handle_input(float input_x, float input_y);

            ecs::registry &get_registry() { return _registry; }
            ecs::entity_t get_player() const { return _player; }
            std::vector<ecs::entity_t> get_obstacles() {
                std::vector<ecs::entity_t> result;
                auto &types = _registry.get_components<component::type>();
                for (std::size_t i = 0; i < types.size(); ++i) {
                    if (types[i] && types[i]->value == component::entity_type::OBSTACLE) {
                        result.push_back(_registry.entity_from_index(i));
                    }
                }
                return result;
            }

        private:
            void update();
            void setup_movement_system();
            void setup_render_system();
            void setup_health_system();
            void setup_enemy_ai_system();
            void check_collisions();

            void draw_entity(std::size_t i);

            // --- Entities ---
            ecs::entity_t _player;
            std::unordered_map<uint32_t, ecs::entity_t> _playerEntities;

            // --- Game state ---
            bool _game_running;
            double _startTime;
            Game &_game;

            // --- Texture cache ---
            std::unordered_map<std::string, Texture2D> _textureCache;
            
    };

} // namespace game::scene

#endif // GAMESCENE_HPP
