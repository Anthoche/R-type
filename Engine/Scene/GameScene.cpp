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
        
        // Enregistrement des composants
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
        _registry.register_component<component::client_id>();

        _game.getGameClient().sendSceneState(SceneState::GAME, &_registry);

        // Configuration des systèmes
        setup_movement_system();
        setup_render_system();
        setup_health_system();
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height));
        game::entities::setup_hitbox_sync_system(_registry);
        
        // Création des entités statiques (background, UI, etc.)
        game::entities::create_background(_registry, 400.f, 300.f, 800.f, 600.f, "assets/background.png", 1.f);
        game::entities::create_sound(_registry, "assets/music.ogg", 0.5f, true, true);
        game::entities::create_text(_registry, {20.f, 30.f}, "R-Type", WHITE, 1.0f, 32);
        game::entities::create_random_element(_registry, 600.f, 450.f, 64.f, 64.f, "assets/items/star.png", "assets/sfx/pickup.wav", 0.8f, false, false);
        
        // Indexer les entités existantes dans le registre
        index_existing_entities();
    }

    void GameScene::index_existing_entities() {
        auto &types = _registry.get_components<component::type>();
        for (std::size_t i = 0; i < types.size(); ++i) {
            if (!types[i]) continue;
            ecs::entity_t entity = _registry.entity_from_index(i);
            
            switch (types[i]->value) {
                case component::entity_type::PLAYER:
                    break;
                case component::entity_type::ENEMY:
                    _enemys.push_back(entity);
                    break;
                case component::entity_type::OBSTACLE:
                    _obstacles.push_back(entity);
                    break;
                default:
                    break;
            }
        }
    }

    void GameScene::update() {
        if (!_game_running) return;
        
        std::unordered_map<uint32_t, std::pair<float, float>> netPlayers;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            netPlayers = _game.getGameClient().players;
        }
        for (auto it = _playerEntities.begin(); it != _playerEntities.end(); ) {
            if (netPlayers.find(it->first) == netPlayers.end()) { 
                _registry.kill_entity(it->second); 
                it = _playerEntities.erase(it); 
            } else { 
                ++it;
            }
        }
        
        // Synchroniser les positions des joueurs
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
                    float currentX = positions[e.value()]->x;
                    float currentY = positions[e.value()]->y;
                    float diffX = x - currentX;
                    float diffY = y - currentY;
                    const float threshold = 0.5f;
                    
                    if (std::abs(diffX) > threshold || std::abs(diffY) > threshold) {
                        float lerpFactor = 0.3f;
                        positions[e.value()]->x = currentX + diffX * lerpFactor;
                        positions[e.value()]->y = currentY + diffY * lerpFactor;
                    } else {
                        positions[e.value()]->x = x;
                        positions[e.value()]->y = y;
                    }
                }
            }
        }
        
        uint32_t myClientId = _game.getGameClient().clientId;
        auto myPlayerIt = _playerEntities.find(myClientId);
        if (myPlayerIt != _playerEntities.end())
            _player = myPlayerIt->second;
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
        
        _isDead = (_game.getGameClient().players.find(_game.getGameClient().clientId) == _game.getGameClient().players.end());

        render_entities();
        
        render_network_obstacles();
        
        render_network_enemies();
        
        render_network_projectiles();
        
        // Écran de mort
        if (_isDead) {
            render_death_screen();
        }
        
        _raylib.endDrawing();
    }

    void GameScene::render_entities() {
        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &types = _registry.get_components<component::type>();
        
        for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
            if (!positions[i] || !drawables[i] || !types[i]) continue;
            
            ecs::entity_t entity = _registry.entity_from_index(i);
            
            switch (types[i]->value) {
                case component::entity_type::PLAYER:
                    render_player(entity, *positions[i], *drawables[i]);
                    break;
                    
                case component::entity_type::ENEMY:
                    render_enemy(entity, *positions[i], *drawables[i]);
                    break;
                    
                case component::entity_type::OBSTACLE:
                    render_obstacle(entity, *positions[i], *drawables[i]);
                    break;
                    
                case component::entity_type::BACKGROUND:
                    render_background(entity, *positions[i], *drawables[i]);
                    break;
                    
                case component::entity_type::TEXT:
                    render_text(entity, *positions[i]);
                    break;
                    
                case component::entity_type::POWERUP:
                    render_powerup(entity, *positions[i], *drawables[i]);
                    break;
                    
                case component::entity_type::PROJECTILE:
                    render_projectile(entity, *positions[i], *drawables[i]);
                    break;
                    
                default:
                    break;
            }
        }
    }

    void GameScene::render_player(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        uint32_t idForColor = 0;
        for (auto const &kv : _playerEntities) {
            if (kv.second == entity) {
                idForColor = kv.first;
                break;
            }
        }
        
        Color playerColor = get_color_for_id(idForColor);
        _raylib.drawRectangle(
            (int)(pos.x - draw.width / 2),
            (int)(pos.y - draw.height / 2),
            (int)draw.width,
            (int)draw.height,
            playerColor
        );
    }

    void GameScene::render_enemy(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        _raylib.drawRectangle(
            (int)(pos.x - draw.width / 2),
            (int)(pos.y - draw.height / 2),
            (int)draw.width,
            (int)draw.height,
            RED
        );
    }

    void GameScene::render_obstacle(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        _raylib.drawRectangle(
            (int)(pos.x - draw.width / 2),
            (int)(pos.y - draw.height / 2),
            (int)draw.width,
            (int)draw.height,
            GRAY
        );
    }

    void GameScene::render_background(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        auto &sprites = _registry.get_components<component::sprite>();
        if (entity.value() < sprites.size() && sprites[entity.value()]) {
            // Afficher avec le sprite
            // TODO: Implémenter l'affichage de sprite
        }
    }

    void GameScene::render_text(ecs::entity_t entity, const component::position &pos) {
        auto &texts = _registry.get_components<component::text>();
        if (entity.value() < texts.size() && texts[entity.value()]) {
            auto &txt = *texts[entity.value()];
            _raylib.drawText(
                txt.content.c_str(),
                (int)pos.x,
                (int)pos.y,
                txt.font_size,
                txt.color
            );
        }
    }

    void GameScene::render_powerup(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        _raylib.drawRectangle(
            (int)(pos.x - draw.width / 2),
            (int)(pos.y - draw.height / 2),
            (int)draw.width,
            (int)draw.height,
            GOLD
        );
    }

    void GameScene::render_projectile(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        _raylib.drawRectangle(
            (int)(pos.x - draw.width / 2),
            (int)(pos.y - draw.height / 2),
            (int)draw.width,
            (int)draw.height,
            WHITE
        );
    }

    void GameScene::render_network_obstacles() {
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
    }

    void GameScene::render_network_enemies() {
        const float ENEMY_WIDTH = 30.f;
        const float ENEMY_HEIGHT = 30.f;
        
        for (auto const &kv : _game.getGameClient().enemies) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            _raylib.drawRectangle(
                (int)(x - ENEMY_WIDTH / 2),
                (int)(y - ENEMY_HEIGHT / 2),
                (int)ENEMY_WIDTH,
                (int)ENEMY_HEIGHT,
                RED
            );
        }
    }

    void GameScene::render_network_projectiles() {
        for (auto &kv : _game.getGameClient().projectiles) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            _raylib.drawRectangle((int)(x - 5), (int)(y - 2), 10, 5, WHITE);
        }
    }

    void GameScene::render_death_screen() {
        _raylib.drawRectangle(0, 0, _width, _height, Color{255, 0, 0, 100});
        
        const char* deathText = "YOU DIED!";
        int fontSize = 72;
        int textWidth = _raylib.measureText(deathText, fontSize);
        _raylib.drawText(
            deathText,
            (_width - textWidth) / 2,
            _height / 2 - fontSize / 2,
            fontSize,
            RED
        );
    }

    Color GameScene::get_color_for_id(uint32_t id) {
        static Color palette[] = {
            RAYWHITE, BLUE, GREEN, YELLOW, ORANGE,
            PURPLE, PINK, GOLD, LIME, SKYBLUE
        };
        return palette[id % (sizeof(palette)/sizeof(palette[0]))];
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
        if (_isDead)
            return;
        auto &positions = _registry.get_components<component::position>();
        auto &controls  = _registry.get_components<component::controllable>();
        auto &hitboxes  = _registry.get_components<component::collision_box>();
        uint32_t myClientId = _game.getGameClient().clientId;
        auto myPlayerIt = _playerEntities.find(myClientId);
        if (myPlayerIt == _playerEntities.end())
            return;
        
        ecs::entity_t myPlayer = myPlayerIt->second;
        
        if (myPlayer.value() < positions.size() && positions[myPlayer.value()] &&
            myPlayer.value() < controls.size() && controls[myPlayer.value()]) {
            float speed = controls[myPlayer.value()]->speed;
            float dt = 0.016f;

            ecs::entity_t playerHitbox = collision::find_player_hitbox(*this);
            if (playerHitbox.value() < hitboxes.size() && hitboxes[playerHitbox.value()]) {
                auto &playerPos = *positions[myPlayer.value()];
                auto &playerBox = *hitboxes[playerHitbox.value()];
                float ix = input_x;
                float iy = input_y;
                float testX = playerPos.x + ix * speed * dt;
                float testY = playerPos.y + iy * speed * dt;

                if (collision::is_blocked(*this, testX, playerPos.y, playerPos, playerBox))
                    ix = 0.f;
                if (collision::is_blocked(*this, playerPos.x, testY, playerPos, playerBox))
                    iy = 0.f;
                if (ix != 0.f || iy != 0.f) {
                    playerPos.x += ix * speed * dt;
                    playerPos.y += iy * speed * dt;
                }

                _game.getGameClient().sendInput(ix, iy);
            }
        }
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

    void GameScene::create_obstacles() {}

    void GameScene::check_collisions() {
        for (auto const &kvPlayer : _playerEntities) {
            collision::handle_entity_collisions(*this, kvPlayer.second);
        }
    }
 
    void GameScene::onClose() {
        _game_running = false;
    }
} // namespace game::scene