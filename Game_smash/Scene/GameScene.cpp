/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** GameScene
*/

#include "Include/GameScene.hpp"
#include <iostream>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <string>
#include <filesystem>
#include <vector>
#include "../../Engine/Physics/Include/Collision.hpp"

namespace game::scene {
    GameScene::GameScene(Game &game)
        : AScene(1920, 1080, "Smash bros"), _player(ecs::entity_t{0}), _game(game), _ui(*this, _registry, _raylib) {
        _raylib = Raylib();
        _game_running = true;
        _startTime = 0.f;
    }

    void GameScene::initialize_player_sprite_frames() {
        _rightFrames.idle = Rectangle{15.f, 13.f, 104.f, 137.f};
        _rightFrames.attack = Rectangle{145.f, 18.f, 112.f, 135.f};
        _rightFrames.hit = Rectangle{274.f, 18.f, 113.f, 124.f};
        _rightFrames.jump = Rectangle{405.f, 16.f, 113.f, 130.f};
        _rightFrames.fall = _rightFrames.jump;

        _leftFrames.idle = Rectangle{20.f, 172.f, 104.f, 138.f};
        _leftFrames.attack = Rectangle{143.f, 177.f, 112.f, 135.f};
        _leftFrames.hit = Rectangle{274.f, 178.f, 113.f, 123.f};
        _leftFrames.jump = Rectangle{408.f, 171.f, 109.f, 139.f};
        _leftFrames.fall = _leftFrames.jump;
    }

    const Rectangle &GameScene::get_player_source_rect(PlayerAnimState state, PlayerFacing facing) const {
        static Rectangle fallback{0.f, 0.f, 0.f, 0.f};
        const PlayerSpriteFrames &frames = (facing == PlayerFacing::Right) ? _rightFrames : _leftFrames;
        switch (state) {
            case PlayerAnimState::Idle:
                return frames.idle.width > 0.f ? frames.idle : fallback;
            case PlayerAnimState::Attack:
                return frames.attack.width > 0.f ? frames.attack : fallback;
            case PlayerAnimState::Hit:
                return frames.hit.width > 0.f ? frames.hit : fallback;
            case PlayerAnimState::Jump:
                return frames.jump.width > 0.f ? frames.jump : fallback;
            case PlayerAnimState::Fall:
                return frames.fall.width > 0.f ? frames.fall : fallback;
        }
        return fallback;
    }

    void GameScene::init() {
        _registry.clear();
        initialize_player_sprite_frames();
        _playerVisualStates.clear();
        _isOpen = true;
        _startTime = _raylib.getTime();
        _raylib.disableCursor();
        _raylib.setTargetFPS(60);
        toggleFullScreen();

        _registry.register_component<component::position>();
        _registry.register_component<component::previous_position>();
        _registry.register_component<component::velocity>();
        _registry.register_component<component::rotation>();
        _registry.register_component<component::scale>();
        _registry.register_component<component::dynamic_position>();
        _registry.register_component<component::drawable>();
        _registry.register_component<component::sprite>();
        _registry.register_component<component::model3D>();
        _registry.register_component<component::audio>();
        _registry.register_component<component::text>();
        _registry.register_component<component::font>();
        _registry.register_component<component::clickable>();
        _registry.register_component<component::hoverable>();
        _registry.register_component<component::controllable>();
        _registry.register_component<component::health>();
        _registry.register_component<component::damage>();
        _registry.register_component<component::collision_box>();
        _registry.register_component<component::hitbox_link>();
        _registry.register_component<component::type>();
        _registry.register_component<component::client_id>();
        _ui.init();
        _game.getGameClient().sendSceneState(SceneState::GAME, &_registry);

        setup_movement_system();
        setup_render_system();
        setup_health_system();
        game::entities::setup_player_control_system(_registry);
        game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height), 0.f);
        game::entities::setup_hitbox_sync_system(_registry);

        game::entities::create_text(_registry, {20.f, 30.f}, "R-Type", WHITE, 1.0f, 32);
        game::entities::create_sound(_registry, std::string(ASSETS_PATH) + "/sounds/Level11.mp3", 0.8f, true, true);
        
        index_existing_entities();
        load_entity_textures();
        load_music();
    }

    void GameScene::index_existing_entities() {
        _player = ecs::entity_t{0};
        _obstacles.clear();
        _platforms.clear();
        _decorations.clear();
        _playerEntities.clear();
        _playerVisualStates.clear();

        auto &types = _registry.get_components<component::type>();
        auto &clientIds = _registry.get_components<component::client_id>();

        for (std::size_t i = 0; i < types.size(); ++i) {
            if (!types[i]) continue;

            ecs::entity_t entity = _registry.entity_from_index(i);

            switch (types[i]->value) {
                case component::entity_type::PLAYER:
                    if (clientIds.size() > i && clientIds[i]) {
                        uint32_t clientId = clientIds[i]->id;
                        _playerEntities.insert_or_assign(clientId, entity);
                        if (clientId == _game.getGameClient().clientId) {
                            _player = entity;
                        }
                    }
                    break;

                case component::entity_type::OBSTACLE:
                    _obstacles.push_back(entity);
                    break;

                case component::entity_type::PLATFORM:
                    _platforms.push_back(entity);
                    break;

                case component::entity_type::DECORATION:
                    _decorations.push_back(entity);
                    break;

                default:
                    break;
            }
        }
    }

    void GameScene::load_entity_textures() {
        auto &sprites = _registry.get_components<component::sprite>();

        for (std::size_t i = 0; i < sprites.size(); ++i) {
            if (sprites[i] && !sprites[i]->image_path.empty()) {
                ecs::entity_t entity = _registry.entity_from_index(i);
                
                if (_entityTextures.find(entity.value()) == _entityTextures.end()) {
                    try {
                        Texture2D texture = _raylib.loadTexture(sprites[i]->image_path);
                        _entityTextures[entity.value()] = texture;
                        std::cout << "[DEBUG] Loaded texture for entity " << entity.value()
                                << " from " << sprites[i]->image_path << std::endl;
                    } catch (const std::exception &e) {
                        std::cerr << "[ERROR] Failed to load texture for entity " << entity.value()
                                << " from " << sprites[i]->image_path << ": " << e.what() << std::endl;
                    }
                }
            }
        }
    }

    void GameScene::unload_entity_textures() {
        for (auto &pair : _entityTextures) {
            _raylib.unloadTexture(pair.second);
        }
        _entityTextures.clear();
        std::cout << "[DEBUG] Unloaded all entity textures" << std::endl;
    }

    Texture2D* GameScene::get_entity_texture(ecs::entity_t entity) {
        auto cachedIt = _entityTextures.find(entity.value());
        if (cachedIt != _entityTextures.end()) {
            return &cachedIt->second;
        }

        auto &sprites = _registry.get_components<component::sprite>();
        if (entity.value() < sprites.size() && sprites[entity.value()]) {
            const std::string &imagePath = sprites[entity.value()]->image_path;
            if (!imagePath.empty()) {
                try {
                    Texture2D texture = _raylib.loadTexture(imagePath);
                    auto [insertIt, inserted] = _entityTextures.emplace(entity.value(), texture);
                    if (inserted) {
                        std::cout << "[DEBUG] Lazily loaded texture for entity "
                                  << entity.value() << " from " << imagePath << std::endl;
                    }
                    return &insertIt->second;
                } catch (const std::exception &e) {
                    std::cerr << "[ERROR] Failed to lazily load texture for entity "
                              << entity.value() << " from " << imagePath
                              << ": " << e.what() << std::endl;
                }
            }
        }
        return nullptr;
    }

    void GameScene::update() {
        if (!_game_running) return;

        std::unordered_map<uint32_t, std::tuple<float, float, float>> netPlayers;
        std::unordered_map<uint32_t, std::pair<int16_t, int16_t>> netHealth;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            netPlayers = _game.getGameClient().players;
            netHealth = _game.getGameClient().playerHealth;
        }

        for (auto it = _playerEntities.begin(); it != _playerEntities.end(); ) {
            if (netPlayers.find(it->first) == netPlayers.end()) {
                _registry.kill_entity(it->second);
                _playerVisualStates.erase(it->first);
                it = _playerEntities.erase(it);
            } else {
                ++it;
            }
        }

        auto &positions = _registry.get_components<component::position>();
        for (auto const &kv : netPlayers) {
            uint32_t id = kv.first;
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            float z = std::get<2>(kv.second);
            auto f = _playerEntities.find(id);
            if (f == _playerEntities.end()) {
                ecs::entity_t e = game::entities::create_player(_registry, x, y, z);
                _playerEntities.emplace(id, e);
                PlayerVisualState &visual = _playerVisualStates[id];
                visual = PlayerVisualState{};
                visual.facing = PlayerFacing::Right;
                visual.previousPosition = Vector2{x, y};
                visual.hasPrevious = true;
                auto healthIt = netHealth.find(id);
                if (healthIt != netHealth.end()) {
                    visual.lastKnownHealth = healthIt->second.first;
                }
            } else {
                ecs::entity_t e = f->second;
                if (e.value() < positions.size() && positions[e.value()]) {
                    positions[e.value()]->x = x;
                    positions[e.value()]->y = y;
                    positions[e.value()]->z = z;
                }
                auto &visual = _playerVisualStates[id];
                if (!visual.hasPrevious) {
                    visual.previousPosition = Vector2{x, y};
                    visual.hasPrevious = true;
                }
                auto healthIt = netHealth.find(id);
                if (healthIt != netHealth.end() && visual.lastKnownHealth < 0) {
                    visual.lastKnownHealth = healthIt->second.first;
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
                pp[i]->z = positions[i]->z;
            }
        }
        update_player_visual_states(netPlayers, netHealth, 1.f / 60.f);
        _registry.run_systems();
    }

    void GameScene::render() {
        _raylib.beginDrawing();
        _raylib.clearBackground(GRAY);
        if (!_isWin)
            _isDead = (_game.getGameClient().players.find(_game.getGameClient().clientId) == _game.getGameClient().players.end());
        _isWin = (30 <= _game.getGameClient().globalScore);
        
        _raylib.updateMusicStream(_music);
        render_entities();
        render_network_obstacles();
        
        _ui.render();
        if (_isDead) {
            render_death_screen();
        } else if (_isWin) {
            render_win_screen();
        }
        if (_isDead && !_defeatSoundPlayed) {
            _raylib.stopMusicStream(_music);
            if (_game.isSoundEnabled()) {
                _raylib.playSound(_defeatSound);
            }
            _defeatSoundPlayed = true;
        }
        if (_isWin && !_victorySoundPlayed) {
            _raylib.stopMusicStream(_music);
            if (_game.isSoundEnabled()) {
                _raylib.playSound(_victorySound);
            }
            _victorySoundPlayed = true;
        }
        render_player_portrait();
        _raylib.endDrawing();
    }

    void GameScene::render_entities() {
        auto &positions = _registry.get_components<component::position>();
        auto &drawables = _registry.get_components<component::drawable>();
        auto &types = _registry.get_components<component::type>();

        for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
            if (!positions[i] || !drawables[i] || !types[i])
                continue;
            if (types[i]->value == component::entity_type::BACKGROUND) {
                ecs::entity_t entity = _registry.entity_from_index(i);
                render_background(entity, *positions[i], *drawables[i]);
            }
        }
        
        std::vector<std::size_t> playerIndices;
        playerIndices.reserve(types.size());

        for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
            if (!positions[i] || !drawables[i] || !types[i]) continue;

            ecs::entity_t entity = _registry.entity_from_index(i);

            switch (types[i]->value) {
                case component::entity_type::DECORATION:
                    render_decoration(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::PLATFORM:
                    render_platform(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::OBSTACLE:
                    render_obstacle(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::POWERUP:
                    render_powerup(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::PLAYER:
                    playerIndices.push_back(i);
                    break;
                default:
                    break;
            }
        }

        for (std::size_t idx : playerIndices) {
            if (idx < positions.size() && positions[idx] && idx < drawables.size() && drawables[idx]) {
                ecs::entity_t entity = _registry.entity_from_index(idx);
                render_player(entity, *positions[idx], *drawables[idx]);
            }
        }
    }

    void GameScene::load_music() {
        _raylib.initAudioDevice();

        auto &audios = _registry.get_components<component::audio>();

        for (std::size_t i = 0; i < audios.size(); ++i) {
            if (!audios[i]) continue;

            const auto &audio = *audios[i];
            if (!std::filesystem::exists(audio.sound_path))
                continue;
            _music = _raylib.loadMusicStream(audio.sound_path);
            _music.looping = audio.loop;
            _raylib.setMusicVolume(_music, audio.volume);
            if (_game.isSoundEnabled()) {
                _raylib.playMusicStream(_music);
            }
            _raylib.updateMusicStream(_music);
            break;
        }
         _shootSound = _raylib.loadSound(std::string(ASSETS_PATH) + "/sounds/shoot.wav");
        _raylib.setSoundVolume(_shootSound, 0.8f);
        _victorySound = _raylib.loadSound(std::string(ASSETS_PATH) + "/sounds/victory.wav");
        _raylib.setSoundVolume(_victorySound, 0.8f);
        _defeatSound = _raylib.loadSound(std::string(ASSETS_PATH) + "/sounds/defeat.wav");
        _raylib.setSoundVolume(_defeatSound, 0.8f);
    }

    void GameScene::render_player(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        uint32_t clientId = 0;
        for (auto const &kv : _playerEntities) {
            if (kv.second == entity) {
                clientId = kv.first;
                break;
            }
        }

        const PlayerVisualState *visualState = nullptr;
        if (clientId != 0) {
            auto it = _playerVisualStates.find(clientId);
            if (it != _playerVisualStates.end()) {
                visualState = &it->second;
            }
        }

        if (texture != nullptr) {
            auto &sprites = _registry.get_components<component::sprite>();
            const component::sprite *spriteComp = nullptr;
            if (entity.value() < sprites.size()) {
                auto &spriteOpt = sprites[entity.value()];
                if (spriteOpt.has_value()) {
                    spriteComp = &spriteOpt.value();
                }
            }

            float scale = (spriteComp && spriteComp->scale > 0.0f) ? spriteComp->scale : 1.0f;
            float rotation = spriteComp ? spriteComp->rotation : 0.0f;

            Rectangle sourceRec;
            bool hasCustomRect = false;
            if (visualState) {
                PlayerFacing renderFacing = visualState->facing;
                if (visualState->state == PlayerAnimState::Hit) {
                    renderFacing = (visualState->facing == PlayerFacing::Right)
                        ? PlayerFacing::Left
                        : PlayerFacing::Right;
                }
                const Rectangle &candidate = get_player_source_rect(visualState->state, renderFacing);
                if (candidate.width > 0.f && candidate.height > 0.f) {
                    sourceRec = candidate;
                    hasCustomRect = true;
                }
            }
            if (!hasCustomRect) {
                sourceRec = Rectangle{0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)};
            }

            float baseWidth = (draw.width > 0.0f) ? draw.width : sourceRec.width;
            float baseHeight = (draw.height > 0.0f) ? draw.height : sourceRec.height;
            float destWidth = baseWidth * scale;
            float destHeight = baseHeight * scale;

            Rectangle destRec = {pos.x - (destWidth / 2.0f), pos.y - (destHeight / 2.0f), destWidth, destHeight};

            Vector2 origin = {0.0f, 0.0f};
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            Color playerColor = get_color_for_id(clientId);
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, playerColor);
        }
    }

    void GameScene::render_obstacle(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            auto &sprites = _registry.get_components<component::sprite>();
            const component::sprite *spriteComp = nullptr;
            if (entity.value() < sprites.size()) {
                auto &spriteOpt = sprites[entity.value()];
                if (spriteOpt.has_value()) {
                    spriteComp = &spriteOpt.value();
                }
            }
            float scale = (spriteComp && spriteComp->scale > 0.0f) ? spriteComp->scale : 1.0f;
            float rotation = spriteComp ? spriteComp->rotation : 0.0f;

            float destWidth = draw.width > 0.0f ? draw.width * scale : static_cast<float>(texture->width) * scale;
            float destHeight = draw.height > 0.0f ? draw.height * scale : static_cast<float>(texture->height) * scale;

            Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)};
            Rectangle destRec = {pos.x - (destWidth / 2.0f), pos.y - (destHeight / 2.0f), destWidth, destHeight};
            Vector2 origin = {0.0f, 0.0f};

            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, GRAY);
        }
    }

    void GameScene::render_platform(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            auto &sprites = _registry.get_components<component::sprite>();
            const component::sprite *spriteComp = nullptr;
            if (entity.value() < sprites.size()) {
                auto &spriteOpt = sprites[entity.value()];
                if (spriteOpt.has_value()) {
                    spriteComp = &spriteOpt.value();
                }
            }
            float scale = (spriteComp && spriteComp->scale > 0.0f) ? spriteComp->scale : 1.0f;
            float rotation = spriteComp ? spriteComp->rotation : 0.0f;

            float destWidth = draw.width > 0.0f ? draw.width * scale : static_cast<float>(texture->width) * scale;
            float destHeight = draw.height > 0.0f ? draw.height * scale : static_cast<float>(texture->height) * scale;

            Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)};
            Rectangle destRec = {pos.x - (destWidth / 2.0f), pos.y - (destHeight / 2.0f), destWidth, destHeight};
            Vector2 origin = {0.0f, 0.0f};

            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, BROWN);
        }
    }

    void GameScene::render_decoration(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            auto &sprites = _registry.get_components<component::sprite>();
            const component::sprite *spriteComp = nullptr;
            if (entity.value() < sprites.size()) {
                auto &spriteOpt = sprites[entity.value()];
                if (spriteOpt.has_value()) {
                    spriteComp = &spriteOpt.value();
                }
            }
            float scale = (spriteComp && spriteComp->scale > 0.0f) ? spriteComp->scale : 1.0f;
            float rotation = spriteComp ? spriteComp->rotation : 0.0f;

            float destWidth = draw.width > 0.0f ? draw.width * scale : static_cast<float>(texture->width) * scale;
            float destHeight = draw.height > 0.0f ? draw.height * scale : static_cast<float>(texture->height) * scale;

            Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)};
            Rectangle destRec = {pos.x - (destWidth / 2.0f), pos.y - (destHeight / 2.0f), destWidth, destHeight};
            Vector2 origin = {0.0f, 0.0f};

            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, PURPLE);
        }
    }

    void GameScene::render_background(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)};
            Rectangle destRec = {0.0f, 0.0f, (float)_width, (float)_height};
            Vector2 origin = {0.0f, 0.0f};
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, 0.0f, WHITE);
        }
    }

    void GameScene::render_powerup(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            auto &sprites = _registry.get_components<component::sprite>();
            const component::sprite *spriteComp = nullptr;
            if (entity.value() < sprites.size()) {
                auto &spriteOpt = sprites[entity.value()];
                if (spriteOpt.has_value()) {
                    spriteComp = &spriteOpt.value();
                }
            }
            float scale = (spriteComp && spriteComp->scale > 0.0f) ? spriteComp->scale : 1.0f;
            float rotation = spriteComp ? spriteComp->rotation : 0.0f;

            float destWidth = draw.width > 0.0f ? draw.width * scale : static_cast<float>(texture->width) * scale;
            float destHeight = draw.height > 0.0f ? draw.height * scale : static_cast<float>(texture->height) * scale;

            Rectangle sourceRec = {0.0f, 0.0f, static_cast<float>(texture->width), static_cast<float>(texture->height)};
            Rectangle destRec = {pos.x - (destWidth / 2.0f), pos.y - (destHeight / 2.0f), destWidth, destHeight};
            Vector2 origin = {0.0f, 0.0f};

            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, GOLD);
        }
    }

    void GameScene::render_network_obstacles() {
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, float, float, float>> obs;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            obs = _game.getGameClient().obstacles;
        }

        for (auto const &kv: obs) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            float w = std::get<3>(kv.second);
            float h = std::get<4>(kv.second);
            _raylib.drawRectangle((int)(x - w / 2), (int)(y - h / 2), (int)w, (int)h, GRAY);
        }
    }

    void GameScene::render_death_screen() {
        _raylib.drawRectangle(0, 0, _width, _height, Color{255, 0, 0, 100});
        bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);

        const char* deathText = isFrench ? "VOUS ÊTES MORT!" : "YOU DIED!";
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

    void GameScene::render_win_screen() {
        _raylib.drawRectangle(0, 0, _width, _height, Color{0, 255, 0, 100});
        
        const char* winText = "YOU WIN!";
        int fontSize = 72;
        int textWidth = _raylib.measureText(winText, fontSize);
        _raylib.drawText(
            winText,
            (_width - textWidth) / 2,
            _height / 2 - fontSize / 2,
            fontSize,
            GREEN
        );
    }

    Color GameScene::get_color_for_id(uint32_t id) {
        static Color palette[] = {
            RAYWHITE, BLUE, GREEN, YELLOW, ORANGE,
            PURPLE, PINK, GOLD, LIME, SKYBLUE
        };
        return palette[id % (sizeof(palette)/sizeof(palette[0]))];
    }

    void GameScene::render_player_portrait() {
        uint32_t myClientId = _game.getGameClient().clientId;
        if (myClientId == 0)
            return;

        auto entityIt = _playerEntities.find(myClientId);
        if (entityIt == _playerEntities.end())
            return;

        ecs::entity_t entity = entityIt->second;
        Texture2D* texture = get_entity_texture(entity);
        if (texture == nullptr)
            return;

        PlayerFacing facing = PlayerFacing::Right;
        auto visualIt = _playerVisualStates.find(myClientId);
        if (visualIt != _playerVisualStates.end()) {
            facing = visualIt->second.facing;
        }

        Rectangle sourceRec = get_player_source_rect(PlayerAnimState::Idle, facing);
        if (sourceRec.width <= 0.f || sourceRec.height <= 0.f) {
            sourceRec = {
                0.f,
                0.f,
                static_cast<float>(texture->width),
                static_cast<float>(texture->height)
            };
        }

        const float outerRadius = 40.f;
        const float innerRadius = 34.f;
        Vector2 center = {
            outerRadius + 28.f,
            outerRadius + 28.f
        };

        _raylib.drawCircle(static_cast<int>(center.x), static_cast<int>(center.y), outerRadius, Color{20, 28, 46, 200});
        _raylib.drawCircle(static_cast<int>(center.x), static_cast<int>(center.y), innerRadius, Color{8, 12, 20, 255});

        const float targetDiameter = innerRadius * 1.5f;
        float scale = 1.f;
        if (sourceRec.width > 0.f && sourceRec.height > 0.f) {
            float fitW = targetDiameter / sourceRec.width;
            float fitH = targetDiameter / sourceRec.height;
            scale = std::min(fitW, fitH);
        }

        Rectangle destRec = {
            center.x - (sourceRec.width * scale) / 2.f,
            center.y - (sourceRec.height * scale) / 2.f,
            sourceRec.width * scale,
            sourceRec.height * scale
        };

        _raylib.drawTexturePro(*texture, sourceRec, destRec, {0.f, 0.f}, 0.f, WHITE);
    }

    void GameScene::update_player_visual_states(
        const std::unordered_map<uint32_t, std::tuple<float, float, float>> &netPlayers,
        const std::unordered_map<uint32_t, std::pair<int16_t, int16_t>> &netHealth,
        float dt) {

        float effectiveDt = (dt > 0.0001f) ? dt : (1.f / 60.f);
        constexpr float horizontalEpsilon = 1.0f;
        constexpr float jumpVelocityThreshold = 140.f;
        constexpr float fallVelocityThreshold = 140.f;
        constexpr float landingVelocityThreshold = 40.f;
        constexpr float landingDisplacementThreshold = 0.6f;
        constexpr float airborneHoldTime = 0.08f;
        constexpr float hitDisplayTime = 0.45f;

        for (auto it = _playerVisualStates.begin(); it != _playerVisualStates.end(); ) {
            if (netPlayers.find(it->first) == netPlayers.end()) {
                it = _playerVisualStates.erase(it);
            } else {
                ++it;
            }
        }

        for (auto const &kv : netPlayers) {
            uint32_t id = kv.first;
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);

            PlayerVisualState &visual = _playerVisualStates[id];
            Vector2 currentPos{x, y};

            if (!visual.hasPrevious) {
                visual.previousPosition = currentPos;
                visual.hasPrevious = true;
            }

            float dx = currentPos.x - visual.previousPosition.x;
            float dy = currentPos.y - visual.previousPosition.y;
            float vy = dy / effectiveDt;

            if (dx > horizontalEpsilon)
                visual.facing = PlayerFacing::Right;
            else if (dx < -horizontalEpsilon)
                visual.facing = PlayerFacing::Left;

            visual.verticalVelocity = vy;

            auto healthIt = netHealth.find(id);
            if (healthIt != netHealth.end()) {
                int currentHealth = healthIt->second.first;
                if (visual.lastKnownHealth >= 0 && currentHealth < visual.lastKnownHealth) {
                    visual.hitTimer = hitDisplayTime;
                    visual.attackTimer = 0.f;
                }
                visual.lastKnownHealth = currentHealth;
            }

            if (visual.hitTimer > 0.f) {
                visual.hitTimer = std::max(0.f, visual.hitTimer - effectiveDt);
            }
            if (visual.attackTimer > 0.f) {
                visual.attackTimer = std::max(0.f, visual.attackTimer - effectiveDt);
            }

            PlayerAnimState newState = visual.state;
            if (visual.hitTimer > 0.f) {
                newState = PlayerAnimState::Hit;
            } else if (visual.attackTimer > 0.f) {
                newState = PlayerAnimState::Attack;
            } else {
                bool goingUp = vy < -jumpVelocityThreshold;
                bool goingDown = vy > fallVelocityThreshold;
                bool slowDescent = dy > landingDisplacementThreshold * 0.5f && vy > landingVelocityThreshold * 0.5f;
                bool canLand = std::fabs(vy) < landingVelocityThreshold &&
                               std::fabs(dy) < landingDisplacementThreshold;

                if (goingUp) {
                    visual.airborne = true;
                    visual.airborneTimer = 0.f;
                    newState = PlayerAnimState::Jump;
                } else if (goingDown || (visual.airborne && slowDescent)) {
                    visual.airborne = true;
                    visual.airborneTimer = 0.f;
                    newState = PlayerAnimState::Fall;
                } else if (visual.airborne) {
                    visual.airborneTimer += effectiveDt;
                    if (visual.state == PlayerAnimState::Fall) {
                        newState = PlayerAnimState::Fall;
                    } else {
                        newState = PlayerAnimState::Jump;
                    }

                    if (canLand && visual.airborneTimer > airborneHoldTime) {
                        visual.airborne = false;
                        visual.airborneTimer = 0.f;
                        newState = PlayerAnimState::Idle;
                    }
                } else {
                    visual.airborne = false;
                    visual.airborneTimer = 0.f;
                    newState = PlayerAnimState::Idle;
                }
            }

            if (newState != visual.state) {
                visual.state = newState;
                visual.stateTimer = 0.f;
            } else {
                visual.stateTimer += effectiveDt;
            }

            visual.previousPosition = currentPos;
        }
    }

    void GameScene::apply_local_attack_state(uint32_t clientId, bool triggerAttack, bool faceLeft, bool faceRight) {
        auto it = _playerVisualStates.find(clientId);
        if (it == _playerVisualStates.end())
            return;

        PlayerVisualState &visual = it->second;

        if (faceLeft)
            visual.facing = PlayerFacing::Left;
        else if (faceRight)
            visual.facing = PlayerFacing::Right;

        if (triggerAttack) {
            constexpr float attackDisplayTime = 0.35f;
            visual.attackTimer = attackDisplayTime;
            if (visual.hitTimer <= 0.f) {
                visual.state = PlayerAnimState::Attack;
                visual.stateTimer = 0.f;
            }
        }
    }

    void GameScene::handleEvents() {
        update();
        int globalScore = 0;
        uint32_t myClientId = 0;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            globalScore = _game.getGameClient().globalScore;
            myClientId = _game.getGameClient().clientId;
        }
        float t = std::clamp(globalScore / 50.0f, 0.0f, 1.0f);
        float SHOOT_COOLDOWN = 0.8f - t * (0.8f - 0.10f);
        bool upPressed = _raylib.isKeyDown(KEY_W) || _raylib.isKeyDown(KEY_UP);
        bool downPressed = _raylib.isKeyDown(KEY_S) || _raylib.isKeyDown(KEY_DOWN);
        bool leftPressed = _raylib.isKeyDown(KEY_A) || _raylib.isKeyDown(KEY_LEFT);
        bool rightPressed = _raylib.isKeyDown(KEY_D) || _raylib.isKeyDown(KEY_RIGHT);
        bool jPressed = _raylib.isKeyDown(KEY_J);
        bool kPressed = _raylib.isKeyDown(KEY_K);

        switch (_raylib.getKeyPressed()) {
            case KEY_F11:
                toggleFullScreen();
                break;
            default:
                break;
        }
        if (_raylib.isGamepadAvailable(0)) {
            float leftStickX = _raylib.getGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            float leftStickY = _raylib.getGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
            if (leftStickY < -0.2f)
                upPressed = true;
            else if (leftStickY > 0.2f)
                downPressed = true;
            if (leftStickX < -0.2f)
                leftPressed = true;
            else if (leftStickX > 0.2f)
                rightPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
                downPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
                upPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
                rightPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
                leftPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
                jPressed = true;
            if (_raylib.isGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT))
                kPressed = true;
        }
        bool wasJDown = _inputState.j;
        bool wasKDown = _inputState.k;
        dispatch_input_events(upPressed, downPressed, leftPressed, rightPressed, jPressed, kPressed);
        if (myClientId != 0) {
            bool attackTriggered = (jPressed && !wasJDown) || (kPressed && !wasKDown);
            bool faceLeft = leftPressed && !rightPressed;
            bool faceRight = rightPressed && !leftPressed;
            apply_local_attack_state(myClientId, attackTriggered, faceLeft, faceRight);
        }
        float input_x = 0.f;
        float input_y = 0.f;
        if (leftPressed != rightPressed)
            input_x = leftPressed ? -1.f : 1.f;
        if (upPressed != downPressed)
            input_y = upPressed ? -1.f : 1.f;
        if (myClientId != 0) {
            if (input_y < 0.f)
                moovePlayer[myClientId] = 33.0f;
            else if (input_y > 0.f)
                moovePlayer[myClientId] = -33.0f;
            else
                moovePlayer[myClientId] = 0.0f;
        }
        handle_input(input_x, input_y, downPressed);
    }

    void GameScene::handle_input(float input_x, float input_y, bool downPressed) {
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

            ecs::entity_t playerHitbox = physics::collision::find_hitbox_of(_registry, myPlayer);
            if (playerHitbox.value() < hitboxes.size() && hitboxes[playerHitbox.value()]) {
                auto &playerPos = *positions[myPlayer.value()];
                auto &playerBox = *hitboxes[playerHitbox.value()];
                float ix = input_x;
                float iy = input_y;
                float testX = playerPos.x + ix * speed * dt;
                float testY = playerPos.y + iy * speed * dt;

                if (physics::collision::is_blocked(_registry, _platforms, testX, playerPos.y, playerBox))
                    ix = 0.f;
                bool allowDrop = downPressed && iy > 0.f;
                if (!allowDrop && physics::collision::is_blocked(_registry, _platforms, playerPos.x, testY, playerBox))
                    iy = 0.f;
                if (ix != 0.f || iy != 0.f) {
                    playerPos.x += ix * speed * dt;
                    playerPos.y += iy * speed * dt;
                }
            }
        }
    }

    void GameScene::dispatch_input_events(bool upPressed, bool downPressed, bool leftPressed, bool rightPressed, bool jPressed, bool kPressed) {
        auto sendIfChanged = [this](InputCode code, bool newState, bool &cachedState) {
            if (newState == cachedState)
                return;
            cachedState = newState;
            _game.getGameClient().sendInputEvent(code, newState);
        };

        sendIfChanged(InputCode::Up, upPressed, _inputState.up);
        sendIfChanged(InputCode::Down, downPressed, _inputState.down);
        sendIfChanged(InputCode::Left, leftPressed, _inputState.left);
        sendIfChanged(InputCode::Right, rightPressed, _inputState.right);
        sendIfChanged(InputCode::J, jPressed, _inputState.j);
        sendIfChanged(InputCode::K, kPressed, _inputState.k);
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
                        pos[i]->z += vel[i]->vz * dt;
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

    void GameScene::toggleFullScreen() {
        int const currentMonitor = _raylib.getCurrentMonitor();

        if (_raylib.isWindowFullscreen()) {
            int posX = (_raylib.getMonitorWidth(currentMonitor) - getWindowWidth()) / 2;
            int posY = (_raylib.getMonitorHeight(currentMonitor) - getWindowHeight()) / 2;
            _raylib.setWindowSize(getWindowWidth() - 1, getWindowHeight() - 1);
            _raylib.setWindowPosition(posX, posY);
        } else {
            _raylib.setWindowSize(getWindowWidth(), getWindowHeight());
        }
        _raylib.setWindowMonitor(currentMonitor);
        _raylib.toggleFullscreen();
    }

    void GameScene::onClose() {
        _raylib.unloadSound(_shootSound);
        _raylib.unloadSound(_victorySound);
        _raylib.unloadSound(_defeatSound);
        _raylib.stopMusicStream(_music);
        _raylib.unloadMusicStream(_music);
        _raylib.closeAudioDevice();
        _game_running = false;
        _ui.unload();
        unload_entity_textures();
    }
} // namespace game::scene
