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

    // ==================== CONSTRUCTEUR ====================
    GameScene::GameScene(Game &game)
        : AScene(800, 600, "R-Type"), _player(ecs::entity_t{0}), _game(game) {
        _raylib = Raylib();
        _game_running = true;
        _startTime = 0.f;
        _game.getGameClient().sendSceneState();
    }

    // ==================== INITIALISATION ====================
    void GameScene::init() {
        _isOpen = true;
        _startTime = _raylib.getTime();
        _raylib.enableCursor();
        _raylib.setTargetFPS(60);
        
        register_components();
        setup_systems();
    }

    void GameScene::register_components() {
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
    }

    void GameScene::setup_systems() {
        setup_movement_system();
        setup_render_system();
        setup_health_system();
        setup_enemy_ai_system();
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height));
        game::entities::setup_hitbox_sync_system(_registry);
    }

    // ==================== UPDATE ====================
    void GameScene::update() {
        if (!_game_running) return;
        
        sync_network_players();
        save_previous_positions();
        _registry.run_systems();
        check_collisions();
    }

    void GameScene::sync_network_players() {
        std::unordered_map<uint32_t, std::pair<float, float>> netPlayers;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            netPlayers = _game.getGameClient().players;
        }
        
        remove_disconnected_players(netPlayers);
        update_player_positions(netPlayers);
        
        if (!_player.value() && !_playerEntities.empty())
            _player = _playerEntities.begin()->second;
    }

    void GameScene::remove_disconnected_players(const std::unordered_map<uint32_t, std::pair<float, float>> &netPlayers) {
        for (auto it = _playerEntities.begin(); it != _playerEntities.end(); ) {
            if (netPlayers.find(it->first) == netPlayers.end()) {
                _registry.kill_entity(it->second);
                it = _playerEntities.erase(it);
            } else {
                ++it;
            }
        }
    }

    void GameScene::update_player_positions(const std::unordered_map<uint32_t, std::pair<float, float>> &netPlayers) {
        auto &positions = _registry.get_components<component::position>();
        
        for (auto const &kv : netPlayers) {
            uint32_t id = kv.first;
            float x = kv.second.first;
            float y = kv.second.second;
            
            auto f = _playerEntities.find(id);
            if (f == _playerEntities.end()) {
                ecs::entity_t e = game::entities::create_player(_registry, x, y);
                _playerEntities.emplace(id, e);
            } else {
                ecs::entity_t e = f->second;
                if (e.value() < positions.size() && positions[e.value()]) {
                    positions[e.value()]->x = x;
                    positions[e.value()]->y = y;
                }
            }
        }
    }

    void GameScene::save_previous_positions() {
        auto &positions = _registry.get_components<component::position>();
        auto &pp = _registry.get_components<component::previous_position>();
        
        for (std::size_t i = 0; i < positions.size() && i < pp.size(); ++i) {
            if (positions[i] && pp[i]) {
                pp[i]->x = positions[i]->x;
                pp[i]->y = positions[i]->y;
            }
        }
    }

    // ==================== RENDER ====================
    void GameScene::render() {
        _raylib.beginDrawing();
        _raylib.clearBackground(BLACK);

        render_all_entities();

        _raylib.endDrawing();
    }

    void GameScene::render_all_entities() {
        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &sprites = _registry.get_components<component::sprite>();
        auto &texts = _registry.get_components<component::text>();
        auto &types = _registry.get_components<component::type>();

        for (std::size_t i = 0; i < positions.size(); ++i) {
            if (!positions[i]) continue;

            ecs::entity_t entity = _registry.entity_from_index(i);
            float x = positions[i]->x;
            float y = positions[i]->y;

            if (i < types.size() && types[i]) {
                render_typed_entity(i, entity, x, y, types[i]->value, drawables, sprites, texts);
            } else {
                render_untyped_entity(i, x, y, drawables, sprites);
            }
        }
    }

    void GameScene::render_typed_entity(std::size_t i, ecs::entity_t entity, float x, float y, 
                                         component::entity_type type,
                                         ecs::sparse_array<component::drawable> &drawables,
                                         ecs::sparse_array<component::sprite> &sprites,
                                         ecs::sparse_array<component::text> &texts) {
        switch (type) {
            case component::entity_type::TEXT:
                render_text(i, x, y, texts);
                break;

            case component::entity_type::PLAYER:
                render_player(i, entity, x, y, drawables, sprites);
                break;

            case component::entity_type::ENEMY:
                render_enemy(i, x, y, drawables, sprites);
                break;

            case component::entity_type::OBSTACLE:
                render_obstacle(i, x, y, drawables, sprites);
                break;

            case component::entity_type::RANDOM_ELEMENT:
                render_random_element(i, x, y, drawables, sprites);
                break;

            case component::entity_type::BACKGROUND:
                render_background(i, x, y, drawables, sprites);
                break;

            default:
                render_default(i, x, y, drawables);
                break;
        }
    }

    void GameScene::render_untyped_entity(std::size_t i, float x, float y,
                                           ecs::sparse_array<component::drawable> &drawables,
                                           ecs::sparse_array<component::sprite> &sprites) {
        if (i < sprites.size() && sprites[i] && i < drawables.size() && drawables[i]) {
            render_sprite(x, y, drawables[i]->width, drawables[i]->height, sprites[i]->texture, WHITE);
        } else if (i < drawables.size() && drawables[i]) {
            render_rectangle(x, y, drawables[i]->width, drawables[i]->height, WHITE);
        }
    }

    // ==================== RENDER HELPERS ====================
    void GameScene::render_text(std::size_t i, float x, float y, ecs::sparse_array<component::text> &texts) {
        if (i < texts.size() && texts[i]) {
            _raylib.drawText(texts[i]->content.c_str(), (int)x, (int)y, texts[i]->font_size, texts[i]->color);
        }
    }

    void GameScene::render_player(std::size_t i, ecs::entity_t entity, float x, float y,
                                   ecs::sparse_array<component::drawable> &drawables,
                                   ecs::sparse_array<component::sprite> &sprites) {
        if (i >= drawables.size() || !drawables[i]) return;

        uint32_t playerId = get_player_network_id(entity);
        Color color = get_color_for_player_id(playerId);

        if (i < sprites.size() && sprites[i]) {
            render_sprite(x, y, drawables[i]->width, drawables[i]->height, sprites[i]->texture, color);
        } else {
            render_rectangle(x, y, drawables[i]->width, drawables[i]->height, color);
        }
    }

    void GameScene::render_enemy(std::size_t i, float x, float y,
                                  ecs::sparse_array<component::drawable> &drawables,
                                  ecs::sparse_array<component::sprite> &sprites) {
        if (i >= drawables.size() || !drawables[i]) return;

        if (i < sprites.size() && sprites[i]) {
            render_sprite(x, y, drawables[i]->width, drawables[i]->height, sprites[i]->texture, WHITE);
        } else {
            render_rectangle(x, y, drawables[i]->width, drawables[i]->height, RED);
        }
    }

    void GameScene::render_obstacle(std::size_t i, float x, float y,
                                     ecs::sparse_array<component::drawable> &drawables,
                                     ecs::sparse_array<component::sprite> &sprites) {
        if (i >= drawables.size() || !drawables[i]) return;

        if (i < sprites.size() && sprites[i]) {
            render_sprite(x, y, drawables[i]->width, drawables[i]->height, sprites[i]->texture, WHITE);
        } else {
            render_rectangle(x, y, drawables[i]->width, drawables[i]->height, GRAY);
        }
    }

    void GameScene::render_random_element(std::size_t i, float x, float y,
                                           ecs::sparse_array<component::drawable> &drawables,
                                           ecs::sparse_array<component::sprite> &sprites) {
        if (i >= drawables.size() || !drawables[i]) return;

        if (i < sprites.size() && sprites[i]) {
            render_sprite(x, y, drawables[i]->width, drawables[i]->height, sprites[i]->texture, WHITE);
        } else {
            render_rectangle(x, y, drawables[i]->width, drawables[i]->height, GOLD);
        }
    }

    void GameScene::render_background(std::size_t i, float x, float y,
                                       ecs::sparse_array<component::drawable> &drawables,
                                       ecs::sparse_array<component::sprite> &sprites) {
        if (i < sprites.size() && sprites[i] && i < drawables.size() && drawables[i]) {
            render_sprite(x, y, drawables[i]->width, drawables[i]->height, sprites[i]->texture, WHITE);
        }
    }

    void GameScene::render_default(std::size_t i, float x, float y,
                                    ecs::sparse_array<component::drawable> &drawables) {
        if (i < drawables.size() && drawables[i]) {
            render_rectangle(x, y, drawables[i]->width, drawables[i]->height, WHITE);
        }
    }

    void GameScene::render_sprite(float x, float y, float width, float height, 
                                   const Texture2D &texture, Color tint) {
        _raylib.drawTexture(texture, (int)(x - width / 2), (int)(y - height / 2), tint);
    }

    void GameScene::render_rectangle(float x, float y, float width, float height, Color color) {
        _raylib.drawRectangle((int)(x - width / 2), (int)(y - height / 2), (int)width, (int)height, color);
    }

    // ==================== UTILITIES ====================
    uint32_t GameScene::get_player_network_id(ecs::entity_t entity) const {
        for (auto const &kv : _playerEntities) {
            if (kv.second == entity) {
                return kv.first;
            }
        }
        return 0;
    }

    Color GameScene::get_color_for_player_id(uint32_t id) const {
        static Color palette[] = {RAYWHITE, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, GOLD, LIME, SKYBLUE};
        return palette[id % (sizeof(palette) / sizeof(palette[0]))];
    }

    // ==================== EVENT HANDLING ====================
    void GameScene::handleEvents() {
        update();
        
        float input_x = 0.f;
        float input_y = 0.f;
        
        if (_raylib.isKeyDown(KEY_W) || _raylib.isKeyDown(KEY_UP))
            input_y = -1.f;
        if (_raylib.isKeyDown(KEY_S) || _raylib.isKeyDown(KEY_DOWN))
            input_y = 1.f;
        if (_raylib.isKeyDown(KEY_A) || _raylib.isKeyDown(KEY_LEFT))
            input_x = -1.f;
        if (_raylib.isKeyDown(KEY_D) || _raylib.isKeyDown(KEY_RIGHT))
            input_x = 1.f;
        
        handle_input(input_x, input_y);
    }

    void GameScene::onClose() {
        _game_running = false;
    }

    void GameScene::handle_input(float input_x, float input_y) {
        _game.getGameClient().sendInput(input_x, input_y);
    }

    // ==================== SYSTEMS ====================
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
                // Système vide, le rendu est géré dans render()
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

    // ==================== ENTITY CREATION (VIDE) ====================
    void GameScene::create_player() {}
    void GameScene::create_obstacles() {}
    void GameScene::create_enemies() {}

    // ==================== COLLISION ====================
    void GameScene::check_collisions() {
        for (auto const &kvPlayer : _playerEntities) {
            collision::handle_entity_collisions(*this, kvPlayer.second);
        }
    }

} // namespace game::scene