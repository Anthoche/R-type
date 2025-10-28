/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameScene
*/


#pragma once

#include "../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../Game.hpp"
#include "../../Shared/protocol.hpp"
#include <unordered_map>
#include "UI.hpp"

namespace game::scene {
    class GameScene : public AScene {
    public:
        Game &_game;

        GameScene(Game &game);

        ~GameScene() override = default;

        void init() override;

        void render() override;

        void handleEvents() override;

        void onClose() override;

        void handle_input(float input_x, float input_y);

        ecs::registry &get_registry() { return _registry; }

        ecs::entity_t get_player() const { return _player; }

        const std::vector<ecs::entity_t> &get_obstacles() const { return _obstacles; }
        
        const std::vector<ecs::entity_t> &get_platforms() const { return _platforms; }

        Game &getGame() { return _game; }

    private:
        struct InputState {
            bool up{false};
            bool down{false};
            bool left{false};
            bool right{false};
            bool j{false};
            bool k{false};
        };

        void update();

        void setup_movement_system();

        void setup_render_system();

        void setup_health_system();

        void load_entity_textures();

        void unload_entity_textures();

        Texture2D* get_entity_texture(ecs::entity_t entity);

        void toggleFullScreen();

        ecs::entity_t _player;
        Music _music;
        Sound _shootSound;
        Sound _victorySound;
        Sound _defeatSound;
        bool _victorySoundPlayed = false;
        bool _defeatSoundPlayed = false;
        std::vector<ecs::entity_t> _obstacles;
        std::vector<ecs::entity_t> _platforms;
        std::vector<ecs::entity_t> _decorations;
        std::unordered_map<uint32_t, ecs::entity_t> _playerEntities;
        bool _isDead = false;
        bool _isWin = false;
        std::unordered_map<uint32_t, Texture2D> _entityTextures;
        std::unordered_map<uint32_t, float> moovePlayer;

        bool _game_running;
        double _startTime;
        UI _ui;

        void index_existing_entities();

        void render_entities();
        void render_player(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_obstacle(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_background(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_powerup(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_platform(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_decoration(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void load_music();

        void render_network_obstacles();
        void render_death_screen();
        void render_win_screen();

        Color get_color_for_id(uint32_t id);
        void dispatch_input_events(bool upPressed, bool downPressed, bool leftPressed, bool rightPressed, bool jPressed, bool kPressed);
        InputState _inputState;
    };
} // namespace game::scene