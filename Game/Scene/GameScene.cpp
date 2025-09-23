/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** GameScene
*/

#include "GameScene.hpp"
#include "../Entities/Include/player.hpp"
#include "../Entities/Include/obstacle.hpp"
#include "../Entities/Include/hitbox.hpp"
#include "../Entities/Include/background.hpp"
#include "../Entities/Include/sound.hpp"
#include "../Entities/Include/text.hpp"
#include "../Entities/Include/random_element.hpp"
#include <iostream>


namespace game::scene {
    GameScene::GameScene(Game &game)
        : AScene(800, 600, "R-Type"), _player(ecs::entity_t{0}), _game(game) {
        _raylib = Raylib();
        _game_running = true;
        _startTime = 0.f;
    }

    void GameScene::init() {
        _isOpen = true;
        _startTime = _raylib.getTime();
        _raylib.enableCursor();
        _raylib.setTargetFPS(60);
        _registry.register_component<component::position>();
        _registry.register_component<component::velocity>();
        _registry.register_component<component::drawable>();
        _registry.register_component<component::controllable>();
        _registry.register_component<component::health>();
        _registry.register_component<component::damage>();
        _registry.register_component<component::type>();
        _registry.register_component<component::collision_box>();
        _registry.register_component<component::hitbox_link>();
        _registry.register_component<component::sprite>();
        _registry.register_component<component::audio>();
        _registry.register_component<component::text>();
        _registry.register_component<component::font>();

        setup_movement_system();
        setup_render_system();
        setup_collision_system();
        setup_health_system();
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height));

        create_player();
        game::entities::create_background(_registry, 400.f, 300.f, 800.f, 600.f, "assets/background.png", 1.f);
        game::entities::create_sound(_registry, "assets/music.ogg", 0.5f, true, true);
        game::entities::create_text(_registry, 20.f, 30.f, "R-Type", 32, "assets/fonts/PressStart2P.ttf");
        game::entities::create_random_element(_registry, 600.f, 450.f, 64.f, 64.f, "assets/items/star.png", "assets/sfx/pickup.wav", 0.8f, false, false);
        create_obstacles();
        game::entities::setup_hitbox_sync_system(_registry);
    }

    void GameScene::update() {
        if (!_game_running) return;
        _registry.run_systems();
        check_collisions();
    }

    void GameScene::render() {
        std::unordered_map<uint32_t, std::pair<float, float>> pls;
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obs;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            pls = _game.getGameClient().players;
            obs = _game.getGameClient().obstacles;
        }

        _raylib.beginDrawing();
        _raylib.clearBackground(BLACK);

        // Obstacles from server
        for (auto const &kv: obs) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            float w = std::get<2>(kv.second);
            float h = std::get<3>(kv.second);
            _raylib.drawRectangle((int)(x - w / 2), (int)(y - h / 2), (int)w, (int)h, GRAY);
        }

        // Players with per-id color
        auto colorForId = [](uint32_t id) -> Color {
            static Color palette[] = {RAYWHITE, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, GOLD, LIME, SKYBLUE};
            return palette[id % (sizeof(palette) / sizeof(palette[0]))];
        };
        for (auto const &kv: pls) {
            _raylib.drawRectangle((int)(kv.second.first - 15), (int)(kv.second.second - 15), 30, 30, colorForId(kv.first));
        }
        _raylib.endDrawing();
    }

    void GameScene::handleEvents() {
        update();
        float input_x = 0.f;
        float input_y = 0.f;
        if (_raylib.isKeyDown(KEY_W) || _raylib.isKeyDown(KEY_UP)) input_y = -1.f;
        if (_raylib.isKeyDown(KEY_S) || _raylib.isKeyDown(KEY_DOWN)) input_y = 1.f;
        if (_raylib.isKeyDown(KEY_A) || _raylib.isKeyDown(KEY_LEFT)) input_x = -1.f;
        if (_raylib.isKeyDown(KEY_D) || _raylib.isKeyDown(KEY_RIGHT)) input_x = 1.f;
        handle_input(input_x, input_y);
    }

    void GameScene::onClose() {
        _game_running = false;
    }

    void GameScene::handle_input(float input_x, float input_y) {
        auto &velocities = _registry.get_components<component::velocity>();
        auto &controllables = _registry.get_components<component::controllable>();
        if (_player.value() < velocities.size() && velocities[_player.value()] &&
            _player.value() < controllables.size() && controllables[_player.value()]) {
            velocities[_player.value()]->vx = input_x * controllables[_player.value()]->speed;
            velocities[_player.value()]->vy = input_y * controllables[_player.value()]->speed;
        }
        _game.getGameClient().sendInput(input_x, input_y);
    }

    void GameScene::setup_movement_system() {
        _registry.add_system<component::position, component::velocity>(
            [](ecs::registry &reg,
               ecs::sparse_array<component::position> &pos,
               ecs::sparse_array<component::velocity> &vel) {
                float dt = 0.016f;
                for (std::size_t i = 0; i < pos.size() && i < vel.size(); ++i) {
                    if (pos[i] && vel[i]) {
                        pos[i]->x += vel[i]->vx * dt;
                        pos[i]->y += vel[i]->vy * dt;
                    }
                }
            });
    }

    void GameScene::setup_render_system() {
        _registry.add_system<component::position, component::drawable>(
            [this](ecs::registry &reg,
                   ecs::sparse_array<component::position> &pos,
                   ecs::sparse_array<component::drawable> &drw) {
            });
    }

    void GameScene::setup_collision_system() {}
    void GameScene::setup_health_system() {
        _registry.add_system<component::health, component::type>(
            [](ecs::registry &reg,
               ecs::sparse_array<component::health> &health,
               ecs::sparse_array<component::type> &type) {
                for (std::size_t i = 0; i < health.size() && i < type.size(); ++i) {
                    if (health[i] && type[i] && health[i]->current <= 0) {
                        ecs::entity_t entity = reg.entity_from_index(i);
                        reg.kill_entity(entity);
                    }
                }
            });
    }

    void GameScene::create_player() {
        _player = game::entities::create_player(_registry, 100.f, 300.f);
    }

    void GameScene::create_obstacles() {
        for (int i = 0; i < 3; ++i) {
            auto obstacle = game::entities::create_obstacle(_registry, 200.f + i * 200.f, 400.f);
            _obstacles.push_back(obstacle);
        }
    }

    void GameScene::check_collisions() {}
} // namespace game::scene
