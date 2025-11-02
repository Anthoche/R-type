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
#include <nlohmann/json.hpp>
#include "../../Engine/Utils/Include/serializer.hpp"
#include <vector>
#include <filesystem>
#include "WeaponDefinition.hpp"

namespace game::scene {
    void GameScene::render() {
        _raylib.beginDrawing();
        _raylib.clearBackground(GRAY);
        if (!_isWin)
            _isDead = (_game.getGameClient().players.find(_game.getGameClient().clientId) == _game.getGameClient().players.end());
        _isWin = _isWin = _game.getGameClient().bossDefeated.load();
        _lastBoss = _game.getGameClient()._lastBoss;
        
        _raylib.updateMusicStream(_music);
        render_entities();
        render_network_projectiles();
        render_network_enemy_projectiles();
        
        _ui.render();
        if (_isDead) {
            render_death_screen();
        } else if (_isWin && !_lastBoss) {
            render_win_screen();
        } else if (_isWin && _lastBoss) {
            render_final_win_screen();
        }
        {
            auto newMessages = _game.getGameClient().consumeChatMessages();
            for (auto &msg : newMessages) {
                _chat.addMessage(msg.first, msg.second);
            }
        }
        _chat.render();
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
            _victoryStartTime = _raylib.getTime();
            _stopShoot = true;
        }

        if (_isWin && _victorySoundPlayed) {
            float currentTime = _raylib.getTime();
            float timeSinceVictory = currentTime - _victoryStartTime;
            
            if (timeSinceVictory >= 6.0f && !_lastBoss) {
                _isWin = false;
                _stopShoot = false;
                _victorySoundPlayed = false;
                _game.getGameClient().bossDefeated.store(false);
                _levelReloadPending = true;
            }
        }

        if (_levelReloadPending) {
            if (processPendingFullRegistry()) {
                _levelReloadPending = false;
                if (_game.isSoundEnabled()) {
                    _raylib.playMusicStream(_music);
                }
            }
        }
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
        
        for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
            if (!positions[i] || !drawables[i] || !types[i]) continue;

            ecs::entity_t entity = _registry.entity_from_index(i);

            switch (types[i]->value) {
                case component::entity_type::OBSTACLE:
                    render_obstacle(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::ENEMY:
                    render_enemy(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::RANDOM_ELEMENT:
                    render_powerup(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::PROJECTILE:
                    render_projectile(entity, *positions[i], *drawables[i]);
                    break;
                case component::entity_type::PLAYER:
                    render_player(entity, *positions[i], *drawables[i]);
                    break;
                default:
                    break;
            }
        }
    }

    void GameScene::render_network_projectiles() {
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, uint32_t>>  projs;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            projs = _game.getGameClient().projectiles;
        }
        
        auto it = _projectileTextures.find("player_missile");
        bool hasTexture = (it != _projectileTextures.end());
        
        for (auto &kv : projs) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            
            if (hasTexture) {
                Texture2D &texture = it->second;
                
                Rectangle sourceRec = {
                    170.0f, 135.0f, 50.0f, 15.0f 
                };
                
                Rectangle destRec = {
                    x - 23.0f, y - 5.0f, 50.0f, 15.0f
                };
                
                Vector2 origin = {0.0f, 0.0f};
                float rotation = 0.0f;
                
                _raylib.drawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
            } else {
                _raylib.drawRectangle((int)(x - 8), (int)(y - 2), 16, 5, WHITE);
            }
        }
    }

    void GameScene::render_player(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            uint32_t clientId = 0;
            for (auto const &kv : _playerEntities) {
                if (kv.second == entity) {
                    clientId = kv.first;
                    break;
                }
            }
            float spriteOffsetX = 0.0f;
            if (moovePlayer.find(clientId) != moovePlayer.end()) {
                spriteOffsetX = moovePlayer[clientId];
            }
            float spriteOffsetY = 0.0f + (17.0f * (clientId % 4));
            Rectangle sourceRec = {66.0f + spriteOffsetX, spriteOffsetY, (float)draw.width, (float)draw.height};
            Rectangle destRec = {pos.x - (draw.width / 2), pos.y - (draw.height / 2), (float)_width / 40.0f, (float)_height / 40.0f};
            Vector2 origin = {0.0f, 0.0f};
            
            auto &sprites = _registry.get_components<component::sprite>();
            float rotation = 0.0f;
            float scale = 1.0f;
            if (entity.value() < sprites.size() && sprites[entity.value()]) {
                scale = sprites[entity.value()]->scale;
                rotation = sprites[entity.value()]->rotation;
            }
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            uint32_t idForColor = 0;
            for (auto const &kv : _playerEntities) {
                if (kv.second == entity) {
                    idForColor = kv.first;
                    break;
                }
            }
            Color playerColor = get_color_for_id(idForColor);
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, playerColor);
        }
    }

    void GameScene::render_enemy(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        static std::unordered_map<uint32_t, float> spriteOffsets;
        static std::unordered_map<uint32_t, float> lastFrameTime;
        
        if (texture != nullptr) {
            uint32_t entityId = entity.value();
            if (spriteOffsets.find(entityId) == spriteOffsets.end()) {
                spriteOffsets[entityId] = 0.0f;
                lastFrameTime[entityId] = 0.0f;
            }
            float currentTime = _raylib.getTime();
            const float FRAME_DURATION = 0.3f;
            const int NUM_FRAMES = 3;
            if (currentTime - lastFrameTime[entityId] >= FRAME_DURATION) {
                spriteOffsets[entityId] += draw.width;
                if (spriteOffsets[entityId] >= draw.width * NUM_FRAMES) {
                    spriteOffsets[entityId] = 0.0f;
                }
                lastFrameTime[entityId] = currentTime;
            }
            Rectangle sourceRec = {spriteOffsets[entityId], 0.0f, (float)draw.width, (float)draw.height};
            Rectangle destRec = {pos.x - (draw.width / 2), pos.y - (draw.height / 2), draw.width, draw.height};
            Vector2 origin = {0.0f, 0.0f};
            
            auto &sprites = _registry.get_components<component::sprite>();
            float rotation = 0.0f;
            if (entity.value() < sprites.size() && sprites[entity.value()]) {
                rotation = sprites[entity.value()]->rotation;
            }
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, YELLOW);
        }
    }

    void GameScene::render_obstacle(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            Rectangle sourceRec = {0.0f, 0.0f, (float)draw.width, (float)draw.height};
            Rectangle destRec = {pos.x - (draw.width / 2), pos.y - (draw.height / 2), draw.width, draw.height};
            Vector2 origin = {0.0f, 0.0f};
            
            auto &sprites = _registry.get_components<component::sprite>();
            float rotation = 0.0f;
            if (entity.value() < sprites.size() && sprites[entity.value()]) {
                rotation = sprites[entity.value()]->rotation;
            }
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, GRAY);
        }
    }

    void GameScene::render_background(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            _backgroundScrollX -= 0.2f;
            if (_backgroundScrollX <= -(float)_width)
                _backgroundScrollX = 0.0f;
            Rectangle sourceRec = {0.0f, 0.0f, (float)draw.width, (float)draw.height};
            Rectangle destRec = {_backgroundScrollX, 0.0f, (float)_width, (float)_height};
            Vector2 origin = {0.0f, 0.0f};
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, 0.0f, WHITE);
            Rectangle destRec2 = {_backgroundScrollX + _width, 0.0f, (float)_width, (float)_height};
            _raylib.drawTexturePro(*texture, sourceRec, destRec2, origin, 0.0f, WHITE);
        }
    }

    void GameScene::render_powerup(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            Rectangle sourceRec = {0.0f, 0.0f, (float)draw.width, (float)draw.height};
            Rectangle destRec = {pos.x - (draw.width / 2), pos.y - (draw.height / 2), draw.width, draw.height};
            Vector2 origin = {0.0f, 0.0f};
            
            auto &sprites = _registry.get_components<component::sprite>();
            float rotation = 0.0f;
            if (entity.value() < sprites.size() && sprites[entity.value()]) {
                rotation = sprites[entity.value()]->rotation;
            }
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            std::cout << "[WARNING] Element has NO texture, drawing fallback rectangle" << std::endl;
            _raylib.drawRectangle(
                (int)(pos.x - draw.width / 2), 
                (int)(pos.y - draw.height / 2), 
                (int)draw.width, 
                (int)draw.height, 
                GOLD
            );            
        }
    }

    void GameScene::render_projectile(ecs::entity_t entity, const component::position &pos, const component::drawable &draw) {
        Texture2D* texture = get_entity_texture(entity);

        if (texture != nullptr) {
            Rectangle sourceRec = {0.0f, 0.0f, (float)draw.width, (float)draw.height};
            Rectangle destRec = {pos.x - (draw.width / 2), pos.y - (draw.height / 2), draw.width, draw.height};
            Vector2 origin = {0.0f, 0.0f};
            
            auto &sprites = _registry.get_components<component::sprite>();
            float rotation = 0.0f;
            if (entity.value() < sprites.size() && sprites[entity.value()]) {
                rotation = sprites[entity.value()]->rotation;
            }
            _raylib.drawTexturePro(*texture, sourceRec, destRec, origin, rotation, WHITE);
        } else {
            _raylib.drawRectangle((int)(pos.x - draw.width / 2), (int)(pos.y - draw.height / 2), (int)draw.width, (int)draw.height, YELLOW);
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

    void GameScene::render_network_enemy_projectiles() {
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, uint32_t>> enemyProjs;
        {
            std::lock_guard<std::mutex> g(_game.getGameClient().stateMutex);
            enemyProjs = _game.getGameClient().enemyProjectiles;
        }
        
        auto it = _projectileTextures.find("enemy_missile");
        bool hasTexture = (it != _projectileTextures.end());
        
        for (auto &kv : enemyProjs) {
            float x = std::get<0>(kv.second);
            float y = std::get<1>(kv.second);
            
            if (hasTexture) {
                Texture2D &texture = it->second;
                
                Rectangle sourceRec = {
                    170.0f, 135.0f, 50.0f, 15.0f 
                };
                
                Rectangle destRec = {
                    x - 23.0f, y - 5.0f, 50.0f, 15.0f
                };
                
                Vector2 origin = {0.0f, 0.0f};
                float rotation = 180.0f;
                
                _raylib.drawTexturePro(texture, sourceRec, destRec, origin, rotation, RED);
            } else {
                _raylib.drawRectangle((int)(x - 8), (int)(y - 2), 16, 5, RED);
            }
        }
    }

    void GameScene::render_death_screen() {
        _raylib.drawRectangle(0, 0, _width, _height, Color{255, 0, 0, 100});
        bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);
        bool isItalian = (_game.getLanguage() == Game::Language::ITALIAN);

        const char* deathText = 
            isFrench ? "VOUS ÊTES MORT!" :
            isItalian ? "SEI MORTO!"
            : "YOU ARE DEAD!";
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
        bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);
        bool isItalian = (_game.getLanguage() == Game::Language::ITALIAN);

        const char* winText = 
            isFrench ? "NIVEAU TERMINÉ !" :
            isItalian ? "LIVELLO SUPERATO !"
            : "LEVEL CLEARED !";
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

    void GameScene::render_final_win_screen() {
        _raylib.drawRectangle(0, 0, _width, _height, Color{255, 255, 0, 100});
        
        const char* winText = "YOU WIN!";
        int fontSize = 72;
        int textWidth = _raylib.measureText(winText, fontSize);
        _raylib.drawText(
            winText,
            (_width - textWidth) / 2,
            _height / 2 - fontSize / 2,
            fontSize,
            YELLOW
        );
    }
} // namespace game::scene
