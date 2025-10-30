/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Ui
*/

#include "Include/UI.hpp"
#include "image.hpp"
#include "RenderUtils.hpp"
#include "Include/GameScene.hpp"
#include "../../../Game.hpp"
#include <algorithm>

UI::UI(game::scene::GameScene &scene, ecs::registry &reg, Raylib &raylib) : _scene(scene), _reg(reg), _raylib(raylib) {
	_margin = Vector2{25, 25};
	_fontSize = 20;
	_spacing = -1.f;
	_heartScale = 1.8f;
	_heartSpacing = 10;
	maxPlayerLives = 4;
	_healthTextGap = 10.f;
}

void UI::init() {
	_font = _raylib.loadFont(ASSETS_PATH"/fonts/PressStart2P.ttf");
	Image heartImage = LoadImage(ASSETS_PATH"/sprites/heart.png");
	Image heartEmptyImage = LoadImage(ASSETS_PATH"/sprites/heart_empty.png");
	_fullHeart = LoadTextureFromImage(heartImage);
	_emptyHeart = LoadTextureFromImage(heartEmptyImage);

	UnloadImage(heartImage);
	UnloadImage(heartEmptyImage);

	uint32_t myClientId = _scene._game.getGameClient().clientId;
	int playerHealth = 100;
	int maxHealth = 100;
	auto it = _scene._game.getGameClient().playerHealth.find(myClientId);
	if (it != _scene._game.getGameClient().playerHealth.end()) {
		playerHealth = it->second.first;
		maxHealth = it->second.second;
	}
	maxPlayerLives = std::max(1, (maxHealth + 24) / 25);
	rebuildHearts(maxPlayerLives);
}

void UI::render() {
    int playerHealth = 100;
    int maxHealth = 100;
    {
        std::lock_guard<std::mutex> g(_scene._game.getGameClient().stateMutex);
        uint32_t myClientId = _scene._game.getGameClient().clientId;
        
        auto it = _scene._game.getGameClient().playerHealth.find(myClientId);
        if (it != _scene._game.getGameClient().playerHealth.end()) {
            playerHealth = it->second.first;
            maxHealth = it->second.second;
        }
    }
    if (playerHealth < 0)
		playerHealth = 0;
	int desiredMaxLives = std::max(1, (maxHealth + 24) / 25);
	if (desiredMaxLives != maxPlayerLives) {
		rebuildHearts(desiredMaxLives);
		maxPlayerLives = desiredMaxLives;
	}

    int playerLives = std::clamp((playerHealth + 24) / 25, 0, maxPlayerLives);

    auto &dynamic_pos = _reg.get_components<component::dynamic_position>();
    auto &drawables = _reg.get_components<component::drawable>();
    auto &sprites = _reg.get_components<component::sprite>();

    const float scaledWidth = _fullHeart.width * _heartScale;
    const float scaledHeight = _fullHeart.height * _heartScale;

    for (std::size_t idx = 0; idx < _heartEntities.size(); ++idx) {
        ecs::entity_t entity = _heartEntities[idx];
        auto index = entity.value();
        if (index >= dynamic_pos.size() || !dynamic_pos[index] ||
            index >= drawables.size() || !drawables[index] ||
            index >= sprites.size() || !sprites[index])
            continue;

        Texture2D tex = (static_cast<int>(idx) < playerLives) ? _fullHeart : _emptyHeart;
        sprites[index]->texture = tex;

        Vector2 offset = {dynamic_pos[index]->offsetX, dynamic_pos[index]->offsetY};
        Vector2 size = {drawables[index]->width, drawables[index]->height};
        Vector2 pos = getRealPos(_raylib, dynamic_pos[index]->position, offset, _margin, size);

        _raylib.drawTextureEx(tex, pos, 0.f, _heartScale, WHITE);
    }

    const float heartsWidth = maxPlayerLives * scaledWidth + std::max(0, maxPlayerLives - 1) * _heartSpacing;
    const float baseX = _raylib.getRenderWidth() - _margin.x - heartsWidth;
    const float textY = _margin.y + scaledHeight + _healthTextGap;
    const std::string healthText = std::format("HP: {}/{}", playerHealth, maxHealth);
    Vector2 textSize = _raylib.measureTextEx(_font, healthText, _fontSize, _spacing);
    Vector2 textPos = {baseX + (heartsWidth - textSize.x) * 0.5f, textY};
    {
        Color healthColor = WHITE;
        
        if (playerHealth <= 25) {
            healthColor = RED;
        } else if (playerHealth <= 50) {
            healthColor = ORANGE;
        }
        
        _raylib.drawTextEx(_font, healthText, textPos, _fontSize, _spacing, healthColor);
    }
}

void UI::unload() {
	for (auto entity : _heartEntities) {
		_reg.kill_entity(entity);
	}
	_heartEntities.clear();
	_raylib.unloadFont(_font);
	_raylib.unloadTexture(_fullHeart);
	_raylib.unloadTexture(_emptyHeart);
}

void UI::addScore(int points) { 
	_playerScore += points; 
}

void UI::resetScore() { 
	_playerScore = 0;
}

void UI::rebuildHearts(int desiredLives) {
	for (auto entity : _heartEntities) {
		_reg.kill_entity(entity);
	}
	_heartEntities.clear();

	const float scaledWidth = _fullHeart.width * _heartScale;
	const float scaledHeight = _fullHeart.height * _heartScale;

	for (int i = 0; i < desiredLives; ++i) {
		float offsetX = -static_cast<float>(i) * (scaledWidth + _heartSpacing);
		auto heart = game::entities::create_image(
			_reg,
			_emptyHeart,
			TOP_RIGHT,
			Vector3{offsetX, 0.f, 0.f},
			scaledWidth,
			scaledHeight,
			0.f
		);
		_heartEntities.push_back(heart);
	}
}
