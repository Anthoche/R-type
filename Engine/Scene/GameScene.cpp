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
        _registry.register_component<component::previous_position>();

        setup_movement_system();
        setup_render_system();
        setup_health_system();
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height));
        game::entities::create_background(_registry, 400.f, 300.f, 800.f, 600.f, "assets/background.png", 1.f);
        game::entities::create_sound(_registry, "assets/music.ogg", 0.5f, true, true);
        game::entities::create_text(_registry, {20.f, 30.f}, "R-Type", WHITE, 1.0f, 32);
        game::entities::create_random_element(_registry, 600.f, 450.f, 64.f, 64.f, "assets/items/star.png", "assets/sfx/pickup.wav", 0.8f, false, false);
        create_obstacles();
        create_enemies();
        game::entities::setup_hitbox_sync_system(_registry);
        setup_enemy_ai_system();
    }

    void GameScene::update() {
        if (!_game_running) return;
        std::unordered_map<uint32_t, std::pair<float, float>> netPlayers;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            netPlayers = _game.getGameClient().players;
        }
        for (auto it = _playerEntities.begin(); it != _playerEntities.end(); ) {
            if (netPlayers.find(it->first) == netPlayers.end()) { _registry.kill_entity(it->second); it = _playerEntities.erase(it); }
            else { ++it; }
        }
        auto &positions = _registry.get_components<component::position>();
        for (auto const &kv : netPlayers) {
            uint32_t id = kv.first; float x = kv.second.first; float y = kv.second.second;
            auto f = _playerEntities.find(id);
            if (f == _playerEntities.end()) {
                ecs::entity_t e = game::entities::create_player(_registry, x, y);
                _playerEntities.emplace(id, e);
            } else {
                ecs::entity_t e = f->second; if (e.value() < positions.size() && positions[e.value()]) { positions[e.value()]->x = x; positions[e.value()]->y = y; }
            }
        }
        if (!_player.value() && !_playerEntities.empty()) _player = _playerEntities.begin()->second;
        auto &pp = _registry.get_components<component::previous_position>();
        for (std::size_t i = 0; i < positions.size() && i < pp.size(); ++i) { 
            if (positions[i] && pp[i]) { 
                pp[i]->x = positions[i]->x; pp[i]->y = positions[i]->y; 
            } 
        }
        check_projectile_enemy_collisions();
        _registry.run_systems();
        check_collisions();
    }

    void GameScene::render() {
        _raylib.beginDrawing();
        _raylib.clearBackground(BLACK);

        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obs;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            obs = _game.getGameClient().obstacles;
        }
        for (auto const &kv: obs) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            float w = std::get<2>(kv.second);
            float h = std::get<3>(kv.second);
            _raylib.drawRectangle((int)(x - w / 2), (int)(y - h / 2), (int)w, (int)h, GRAY);
        }

        auto colorForId = [](uint32_t id) -> Color { 
            static Color palette[] = {RAYWHITE, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, GOLD, LIME, SKYBLUE}; 
            return palette[id % (sizeof(palette)/sizeof(palette[0]))]; 
        };

        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &types = _registry.get_components<component::type>();
        for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
            if (!positions[i] || !drawables[i] || !types[i]) continue;
            ecs::entity_t e = _registry.entity_from_index(i);
            if (types[i]->value == component::entity_type::PLAYER) {
                uint32_t idForColor = 0;
                for (auto const &kv : _playerEntities) {
                    if (kv.second == e) {
                        idForColor = kv.first;
                        break;
                    }
                }
                _raylib.drawRectangle(
                    (int)(positions[i]->x - drawables[i]->width / 2),
                    (int)(positions[i]->y - drawables[i]->height / 2),
                    (int)drawables[i]->width,
                    (int)drawables[i]->height,
                    colorForId(idForColor)
                );
            }
            else if (types[i]->value == component::entity_type::ENEMY) {
                _raylib.drawRectangle(
                    (int)(positions[i]->x - drawables[i]->width / 2),
                    (int)(positions[i]->y - drawables[i]->height / 2),
                    (int)drawables[i]->width,
                    (int)drawables[i]->height,
                    RED
                );
            }
        }
        for (auto &kv : _game.getGameClient().projectiles) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            _raylib.drawRectangle((int)(x - 5), (int)(y - 2), 10, 5, WHITE);
        }
        _raylib.endDrawing();
    }

    void GameScene::handleEvents() {
        update();
        float input_x = 0.f;
        float input_y = 0.f;
        static float lastShotTime = 0.f;
        const float SHOOT_COOLDOWN = 0.3f;

        if (_raylib.isKeyDown(KEY_W) || _raylib.isKeyDown(KEY_UP))
            input_y = -1.f;
        if (_raylib.isKeyDown(KEY_S) || _raylib.isKeyDown(KEY_DOWN))
            input_y = 1.f;
        if (_raylib.isKeyDown(KEY_A) || _raylib.isKeyDown(KEY_LEFT))
            input_x = -1.f;
        if (_raylib.isKeyDown(KEY_D) || _raylib.isKeyDown(KEY_RIGHT))
            input_x = 1.f;
        if (_raylib.isKeyPressed(KEY_SPACE)) {
            float currentTime = _raylib.getTime();
            if (currentTime - lastShotTime >= SHOOT_COOLDOWN) {
                handle_shoot();
                lastShotTime = currentTime;
            }
        }
        handle_input(input_x, input_y);
    }

    void GameScene::handle_shoot() {
        _game.getGameClient().sendShoot();
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
            [this](ecs::registry &reg,
                   ecs::sparse_array<component::position> &pos,
                   ecs::sparse_array<component::drawable> &drw) {
            });
    }

    void GameScene::setup_health_system() {
        _registry.add_system<component::health, component::type>(
            [](ecs::registry &reg,
               ecs::sparse_array<component::health> &health,
               ecs::sparse_array<component::type> &type) {
                for (std::size_t i = 0; i < health.size() && i < type.size(); ++i) {
                    if (health[i] && type[i] && health[i]->current <= 0) {
                        if (type[i]->value == component::entity_type::PLAYER) {
                            reg.remove_component<component::drawable>(reg.entity_from_index(i));
                        } else {
                            ecs::entity_t entity = reg.entity_from_index(i);
                            reg.kill_entity(entity);
                        }
                    }
                }
            });
    }

    void GameScene::create_player() {}

    void GameScene::create_obstacles() {
        for (int i = 0; i < 3; ++i) {
            auto obstacle = game::entities::create_obstacle(_registry, 200.f + i * 200.f, 400.f);
            _obstacles.push_back(obstacle);
        }
    }

    void GameScene::create_enemies() {
        for (int i = 0; i < 5; ++i) {
            auto enemy = game::entities::create_enemy(_registry, 100.f + i * 100.f, 200.f);
            _enemys.push_back(enemy);
        }
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
 
    void GameScene::onClose() {
        _game_running = false;
    }

    void GameScene::check_projectile_enemy_collisions() {
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> localProjs;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            localProjs = _game.getGameClient().projectiles;
        }
        
        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &types = _registry.get_components<component::type>();
        
        for (const auto& proj : localProjs) {
            float projX = std::get<0>(proj.second);
            float projY = std::get<1>(proj.second);
            
            float projLeft = projX - 5.f;
            float projRight = projX + 5.f;
            float projTop = projY - 2.5f;
            float projBottom = projY + 2.5f;
            
            for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
                if (!positions[i] || !drawables[i] || !types[i]) continue;
                
                if (types[i]->value == component::entity_type::ENEMY) {
                    float enemyX = positions[i]->x;
                    float enemyY = positions[i]->y;
                    float enemyW = drawables[i]->width;
                    float enemyH = drawables[i]->height;
                    
                    float enemyLeft = enemyX - enemyW * 0.5f;
                    float enemyRight = enemyX + enemyW * 0.5f;
                    float enemyTop = enemyY - enemyH * 0.5f;
                    float enemyBottom = enemyY + enemyH * 0.5f;
                    
                    if (projRight > enemyLeft && projLeft < enemyRight &&
                        projBottom > enemyTop && projTop < enemyBottom) {                    
                        ecs::entity_t entity = _registry.entity_from_index(i);
                        _registry.kill_entity(entity);                    
                        break;
                    }
                }
            }
        }
    }
} // namespace game::scene