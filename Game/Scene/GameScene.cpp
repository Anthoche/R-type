/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** GameScene
*/

#include "GameScene.hpp"
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
        _registry.register_component<component::font>();
        _registry.register_component<component::previous_position>();

        setup_movement_system();
        setup_render_system();
        setup_collision_system();
        setup_health_system();
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height));
        game::entities::create_background(_registry, 400.f, 300.f, 800.f, 600.f, "assets/background.png", 1.f);
        game::entities::create_sound(_registry, "assets/music.ogg", 0.5f, true, true);
        game::entities::create_text(_registry, 20.f, 30.f, "R-Type", 32, "assets/fonts/PressStart2P.ttf");
        game::entities::create_random_element(_registry, 600.f, 450.f, 64.f, 64.f, "assets/items/star.png", "assets/sfx/pickup.wav", 0.8f, false, false);
        create_obstacles();
        game::entities::setup_hitbox_sync_system(_registry);
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
        for (std::size_t i = 0; i < positions.size() && i < pp.size(); ++i) { if (positions[i] && pp[i]) { pp[i]->x = positions[i]->x; pp[i]->y = positions[i]->y; } }
        _registry.run_systems();
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

        auto colorForId = [](uint32_t id) -> Color { static Color palette[] = {RAYWHITE, BLUE, GREEN, YELLOW, ORANGE, PURPLE, PINK, GOLD, LIME, SKYBLUE}; return palette[id % (sizeof(palette)/sizeof(palette[0]))]; };
        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &types = _registry.get_components<component::type>();
        for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
            if (!positions[i] || !drawables[i] || !types[i]) continue;
            if (types[i]->value != component::entity_type::PLAYER) continue;
            uint32_t idForColor = 0; ecs::entity_t e = _registry.entity_from_index(i);
            for (auto const &kv : _playerEntities) { if (kv.second == e) { idForColor = kv.first; break; } }
            _raylib.drawRectangle(
                (int)(positions[i]->x - drawables[i]->width / 2),
                (int)(positions[i]->y - drawables[i]->height / 2),
                (int)drawables[i]->width,
                (int)drawables[i]->height,
                colorForId(idForColor)
            );
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
        float ix = input_x;
        float iy = input_y;
        auto &positions = _registry.get_components<component::position>();
        auto &controls  = _registry.get_components<component::controllable>();
        auto &hitboxes  = _registry.get_components<component::collision_box>();

        if (_player.value() < positions.size() && positions[_player.value()] &&
            _player.value() < controls.size() && controls[_player.value()]) {
            float speed = controls[_player.value()]->speed;
            ecs::entity_t playerHitbox = find_player_hitbox();
            if (playerHitbox.value() < hitboxes.size() && hitboxes[playerHitbox.value()]) {
                auto &playerPos = *positions[_player.value()];
                auto &playerBox = *hitboxes[playerHitbox.value()];
                float dt = 0.016f;
                float testX = playerPos.x + ix * speed * dt;
                if (is_blocked(testX, playerPos.y, playerPos, playerBox))
                    ix = 0.f;
                float testY = playerPos.y + iy * speed * dt;
                if (is_blocked(playerPos.x, testY, playerPos, playerBox))
                    iy = 0.f;
            }
        }
        _game.getGameClient().sendInput(ix, iy);
    }


    ecs::entity_t GameScene::find_player_hitbox() {
        auto &links = _registry.get_components<component::hitbox_link>();
        for (std::size_t i = 0; i < links.size(); ++i) {
            if (links[i] && links[i]->owner == _player) {
                return _registry.entity_from_index(i);
            }
        }
        return ecs::entity_t{0};
    }

    bool GameScene::is_blocked(float testX, float testY,
                           const component::position &playerPos,
                           const component::collision_box &playerBox) {
        float left   = testX - playerBox.width * 0.5f;
        float right  = testX + playerBox.width * 0.5f;
        float top    = testY - playerBox.height * 0.5f;
        float bottom = testY + playerBox.height * 0.5f;
        auto &positions = _registry.get_components<component::position>();
        auto &hitboxes  = _registry.get_components<component::collision_box>();

        for (auto &obstacle : _obstacles) {
            if (obstacle.value() >= positions.size() || !positions[obstacle.value()] || !hitboxes[obstacle.value()])
                continue;
            auto &obsPos = *positions[obstacle.value()];
            auto &obsBox = *hitboxes[obstacle.value()];
            float obsLeft   = obsPos.x - obsBox.width * 0.5f;
            float obsRight  = obsPos.x + obsBox.width * 0.5f;
            float obsTop    = obsPos.y - obsBox.height * 0.5f;
            float obsBottom = obsPos.y + obsBox.height * 0.5f;
            bool overlap = right > obsLeft && left < obsRight && bottom > obsTop && top < obsBottom;
            if (overlap)
                return true;
        }
        return false;
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

    void GameScene::create_player() {}

    void GameScene::create_obstacles() {
        for (int i = 0; i < 3; ++i) {
            auto obstacle = game::entities::create_obstacle(_registry, 200.f + i * 200.f, 400.f);
            _obstacles.push_back(obstacle);
        }
    }

    void GameScene::check_collisions() {
        for (auto const &kvPlayer : _playerEntities) {
            handle_entity_collisions(kvPlayer.second);
        }
    }

    void GameScene::handle_entity_collisions(ecs::entity_t entity) {
        auto &positions     = _registry.get_components<component::position>();
        auto &hitboxes      = _registry.get_components<component::collision_box>();
        auto &prevPositions = _registry.get_components<component::previous_position>();

        if (entity.value() >= positions.size() || !positions[entity.value()])
            return;
        ecs::entity_t entityHitbox = find_hitbox_of(entity);
        if (!entityHitbox.value() || entityHitbox.value() >= hitboxes.size() || !hitboxes[entityHitbox.value()])
            return;
        auto &pos = *positions[entity.value()];
        auto &box = *hitboxes[entityHitbox.value()];
        float prevX = (entity.value() < prevPositions.size() && prevPositions[entity.value()])
                        ? prevPositions[entity.value()]->x
                        : pos.x;
        float prevY = (entity.value() < prevPositions.size() && prevPositions[entity.value()])
                        ? prevPositions[entity.value()]->y
                        : pos.y;
        for (auto &obstacle : _obstacles) {
            if (obstacle.value() >= positions.size() || !positions[obstacle.value()] || !hitboxes[obstacle.value()])
                continue;
            auto &obsPos = *positions[obstacle.value()];
            auto &obsBox = *hitboxes[obstacle.value()];
            float nextLeft   = pos.x - box.width * 0.5f;
            float nextRight  = pos.x + box.width * 0.5f;
            float nextTop    = pos.y - box.height * 0.5f;
            float nextBottom = pos.y + box.height * 0.5f;
            float obsLeft   = obsPos.x - obsBox.width * 0.5f;
            float obsRight  = obsPos.x + obsBox.width * 0.5f;
            float obsTop    = obsPos.y - obsBox.height * 0.5f;
            float obsBottom = obsPos.y + obsBox.height * 0.5f;
            if (!overlap_aabb(nextLeft, nextRight, nextTop, nextBottom,
                            obsLeft, obsRight, obsTop, obsBottom))
                continue;
            resolve_collision(entity, pos, box, obsPos, obsBox, prevX, prevY);
        }
    }

    ecs::entity_t GameScene::find_hitbox_of(ecs::entity_t owner) {
        auto &links = _registry.get_components<component::hitbox_link>();
        for (std::size_t i = 0; i < links.size(); ++i) {
            if (links[i] && links[i]->owner == owner) {
                return _registry.entity_from_index(i);
            }
        }
        return ecs::entity_t{0};
    }

    bool GameScene::overlap_aabb(float leftA, float rightA, float topA, float bottomA,
                             float leftB, float rightB, float topB, float bottomB) {
        return rightA > leftB && leftA < rightB && bottomA > topB && topA < bottomB;
    }

    void GameScene::resolve_collision(ecs::entity_t playerEntity,
                                  component::position &playerPos,
                                  component::collision_box &playerBox,
                                  const component::position &obsPos,
                                  const component::collision_box &obsBox,
                                  float prevX, float prevY) {
        auto &velocitiesAll  = _registry.get_components<component::velocity>();
        auto &prevPositions  = _registry.get_components<component::previous_position>();
        float obsLeft   = obsPos.x - obsBox.width * 0.5f;
        float obsRight  = obsPos.x + obsBox.width * 0.5f;
        float obsTop    = obsPos.y - obsBox.height * 0.5f;
        float obsBottom = obsPos.y + obsBox.height * 0.5f;
        float moveX = playerPos.x - prevX;
        float moveY = playerPos.y - prevY;
        const float skin = 0.5f;

        if (std::fabs(moveX) > std::fabs(moveY)) {
            if (moveX > 0.f)
                playerPos.x = obsLeft - playerBox.width * 0.5f - skin;
            else
                playerPos.x = obsRight + playerBox.width * 0.5f + skin;

            if (playerEntity.value() < velocitiesAll.size() && velocitiesAll[playerEntity.value()])
                velocitiesAll[playerEntity.value()]->vx = 0.f;
        } else {
            if (moveY > 0.f)
                playerPos.y = obsTop - playerBox.height * 0.5f - skin;
            else
                playerPos.y = obsBottom + playerBox.height * 0.5f + skin;

            if (playerEntity.value() < velocitiesAll.size() && velocitiesAll[playerEntity.value()])
                velocitiesAll[playerEntity.value()]->vy = 0.f;
        }
        if (playerEntity.value() < prevPositions.size() && prevPositions[playerEntity.value()]) {
            prevPositions[playerEntity.value()]->x = playerPos.x;
            prevPositions[playerEntity.value()]->y = playerPos.y;
        }
    }
} // namespace game::scene
