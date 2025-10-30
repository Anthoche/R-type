/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Ui
*/

#include "Include/UI.hpp"
#include <algorithm>
#include <cmath>
#include <format>
#include <unordered_map>
#include <vector>
#include "image.hpp"
#include "RenderUtils.hpp"
#include "text.hpp"
#include "Include/GameScene.hpp"
#include "../../../Game.hpp"

UI::UI(game::scene::GameScene &scene, ecs::registry &reg, Raylib &raylib) : _scene(scene), _reg(reg), _raylib(raylib) {
	_margin = Vector2{25, 25};
	_fontSize = 20;
	_spacing = -1.f;
	_heartScale = 1.8f;
	_heartSpacing = 10;
	maxPlayerLives = 3;
}

void UI::init() {
	_font = _raylib.loadFont(ASSETS_PATH"/fonts/PressStart2P.ttf");
	Image heartImage = LoadImage(ASSETS_PATH"/sprites/heart.png");
	Image heartEmptyImage = LoadImage(ASSETS_PATH"/sprites/heart_empty.png");
	_fullHeart = LoadTextureFromImage(heartImage);
	_emptyHeart = LoadTextureFromImage(heartEmptyImage);

	UnloadImage(heartImage);
	UnloadImage(heartEmptyImage);
}

void UI::render() {
    auto &client = _scene._game.getGameClient();
    std::unordered_map<uint32_t, std::pair<int16_t, int16_t>> healthSnapshot;
    uint32_t myClientId = client.clientId;
    {
        std::lock_guard<std::mutex> lock(client.stateMutex);
        healthSnapshot = client.playerHealth;
    }

    const float heartWidth = _fullHeart.width * _heartScale;
    const float heartHeight = _fullHeart.height * _heartScale;
    const float heartStep = heartWidth + _heartSpacing;
    const float hpTextOffset = heartHeight + 12.f;

    auto computeLives = [this](int currentHealth, int maxHealth) -> int {
        if (maxPlayerLives <= 0 || maxHealth <= 0)
            return 0;

        const float unit = std::max(1.f, static_cast<float>(maxHealth) / static_cast<float>(maxPlayerLives));
        int lives = static_cast<int>(std::ceil(std::max(0.f, static_cast<float>(currentHealth)) / unit));
        return std::clamp(lives, 0, maxPlayerLives);
    };

    int myCurrentHealth = 0;
    int myMaxHealth = 0;
    if (auto it = healthSnapshot.find(myClientId); it != healthSnapshot.end()) {
        myCurrentHealth = std::max(0, static_cast<int>(it->second.first));
        myMaxHealth = std::max(0, static_cast<int>(it->second.second));
    }

    int myLives = computeLives(myCurrentHealth, myMaxHealth);

    for (int i = 0; i < maxPlayerLives; ++i) {
        Texture2D &tex = (i < myLives) ? _fullHeart : _emptyHeart;
        Vector2 position = {_margin.x + static_cast<float>(i) * heartStep, _margin.y};
        _raylib.drawTextureEx(tex, position, 0.f, _heartScale, WHITE);
    }

    {
        std::string healthText = std::format("HP: {}/{}", myCurrentHealth, myMaxHealth);
        Vector2 healthPos = {_margin.x, _margin.y + hpTextOffset};
        Color healthColor = WHITE;

        if (myMaxHealth > 0) {
            float ratio = static_cast<float>(myCurrentHealth) / static_cast<float>(std::max(1, myMaxHealth));
            if (ratio <= 0.25f) {
                healthColor = RED;
            } else if (ratio <= 0.5f) {
                healthColor = ORANGE;
            }
        }

        _raylib.drawTextEx(_font, healthText, healthPos, _fontSize, _spacing, healthColor);
    }
}

void UI::unload() {
	_raylib.unloadFont(_font);
	_raylib.unloadTexture(_fullHeart);
	_raylib.unloadTexture(_emptyHeart);
}

Vector2 UI::getRealPos(DynamicPosition pos, Vector2 offset, Vector2 size) const {
	Vector2 finalPos = {0.f, 0.f};
	float bottomPosY = _raylib.getRenderHeight() - _margin.y - (size.y) / 2;
	float centerPosX = getElementCenter(_raylib.getRenderWidth(), size.x);
	float rightPosX = _raylib.getRenderWidth() - size.x - _margin.x;

	switch (pos) {
		case TOP_LEFT:
			finalPos = {_margin.x, _margin.y};
			break;
		case TOP_CENTER:
			finalPos = {centerPosX, _margin.y};
			break;
		case TOP_RIGHT:
			finalPos = {rightPosX, _margin.y};
			break;
		case BOTTOM_LEFT:
			finalPos = {_margin.x, bottomPosY};
			break;
		case BOTTOM_CENTER:
			finalPos = {centerPosX, bottomPosY};
			break;
		case BOTTOM_RIGHT:
			finalPos = {rightPosX, bottomPosY};
			break;
	}
	finalPos.x += offset.x;
	finalPos.y += offset.y;
	return finalPos;
}

Vector2 UI::getTextPos(DynamicPosition pos, Vector2 offset, std::string const &content) const {
	Vector2 textSize = _raylib.measureTextEx(_font, content, _fontSize, _spacing);

	return getRealPos(pos, offset, textSize);
}

void UI::addScore(int points) { 
	_playerScore += points; 
}

void UI::resetScore() { 
	_playerScore = 0;
}
