/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** GameScene
*/

#include "Include/GameScene.hpp"
#include "../Physics/Include/Collision.hpp"
#include <iostream>
#include <cmath>

namespace game::scene {

    GameScene::GameScene(Game &game)
        : AScene(800, 600, "R-Type"), _player(ecs::entity_t{0}), _game(game) {
        _raylib = Raylib();
        _game_running = true;
        _startTime = 0.f;
        _game.getGameClient().sendSceneState();
    }

    void GameScene::init() {
        _isOpen = true;
        _startTime = _raylib.getTime();
        _raylib.enableCursor();
        _raylib.setTargetFPS(60);

        // Components
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
        _registry.register_component<component::previous_position>();

        // Systems
        setup_movement_system();
        setup_render_system();
        setup_health_system();
        setup_enemy_ai_system();
        game::entities::setup_hitbox_sync_system(_registry);
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height));
    }

    void GameScene::update() {
        if (!_game_running) return;

        std::unordered_map<uint32_t, std::pair<float, float>> netPlayers;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            netPlayers = _game.getGameClient().players;
        }

        // Sync players
        auto &positions = _registry.get_components<component::position>();
        for (auto const &kv : netPlayers) {
            uint32_t id = kv.first; 
            float x = kv.second.first; 
            float y = kv.second.second;

            auto f = _playerEntities.find(id);
            if (f != _playerEntities.end()) {
                ecs::entity_t e = f->second; 
                if (e.value() < positions.size() && positions[e.value()]) {
                    positions[e.value()]->x = x; 
                    positions[e.value()]->y = y;
                }
            }
        }

        if (!_player.value() && !_playerEntities.empty()) 
            _player = _playerEntities.begin()->second;

        auto &pp = _registry.get_components<component::previous_position>();
        for (std::size_t i = 0; i < positions.size() && i < pp.size(); ++i) { 
            if (positions[i] && pp[i]) { 
                pp[i]->x = positions[i]->x; 
                pp[i]->y = positions[i]->y; 
            }
        }

        _registry.run_systems();
        check_collisions();
    }

    void GameScene::render() {
        _raylib.beginDrawing();
        _raylib.clearBackground(BLACK);

        // Draw all ECS entities
        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &types = _registry.get_components<component::type>();

        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (positions[i] && drawables[i] && types[i]) {
                draw_entity(i);
            }
        }

        _raylib.endDrawing();
    }

    void GameScene::draw_entity(std::size_t i) {
        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &types = _registry.get_components<component::type>();
        auto &sprites = _registry.get_components<component::sprite>();

        auto type = types[i]->value;

        switch (type) {
            case component::entity_type::PLAYER:
            case component::entity_type::ENEMY:
            case component::entity_type::OBSTACLE:
            case component::entity_type::BACKGROUND: {
                if (i < sprites.size() && sprites[i] && !sprites[i]->image_path.empty()) {
                    const std::string &path = sprites[i]->image_path;

                    if (_textureCache.find(path) == _textureCache.end()) {
                        Texture2D tex = LoadTexture(path.c_str());
                        _textureCache[path] = tex;
                        std::cout << "[GameScene] Loaded texture: " << path << std::endl;
                    }

                    Texture2D tex = _textureCache[path];
                    float w = tex.width * sprites[i]->scale;
                    float h = tex.height * sprites[i]->scale;

                    _raylib.drawTextureEx(
                        tex,
                        {positions[i]->x - w / 2, positions[i]->y - h / 2},
                        sprites[i]->rotation,
                        sprites[i]->scale,
                        WHITE
                    );
                } else {
                    _raylib.drawRectangle(
                        (int)(positions[i]->x - drawables[i]->width / 2),
                        (int)(positions[i]->y - drawables[i]->height / 2),
                        (int)drawables[i]->width,
                        (int)drawables[i]->height,
                        (type == component::entity_type::ENEMY) ? RED : BLUE
                    );
                }
                break;
            }
            case component::entity_type::TEXT: {
                auto &texts = _registry.get_components<component::text>();
                if (i < texts.size() && texts[i]) {
                    _raylib.drawText(
                        texts[i]->content,
                        (int)positions[i]->x,
                        (int)positions[i]->y,
                        texts[i]->font_size,
                        texts[i]->color
                    );
                }
                break;
            }
            default:
                break;
        }
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

        // Libérer toutes les textures
        for (auto &kv : _textureCache) {
            UnloadTexture(kv.second);
            std::cout << "[GameScene] Unloaded texture: " << kv.first << std::endl;
        }
        _textureCache.clear();
    }

    void GameScene::handle_input(float input_x, float input_y) {
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
            [](ecs::registry &reg,
               ecs::sparse_array<component::position> &,
               ecs::sparse_array<component::drawable> &) {
                // Rendering handled in GameScene::render
            });
    }

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

    void GameScene::setup_enemy_ai_system() {
        _registry.add_system<component::position, component::velocity, component::type>(
            [this](ecs::registry &reg,
                   ecs::sparse_array<component::position> &pos,
                   ecs::sparse_array<component::velocity> &vel,
                   ecs::sparse_array<component::type> &type) {
                for (std::size_t i = 0; i < pos.size() && i < vel.size() && i < type.size(); ++i) {
                    if (pos[i] && vel[i] && type[i] &&
                        type[i]->value == component::entity_type::ENEMY) {
                        if (pos[i]->x < 20.f || pos[i]->x > _width - 20.f) {
                            vel[i]->vx *= -1.f;
                        }
                    }
                }
            });
    }

    void GameScene::check_collisions() {
        for (auto const &kvPlayer : _playerEntities) {
            collision::handle_entity_collisions(*this, kvPlayer.second);
        }
    }

} // namespace game::scene
