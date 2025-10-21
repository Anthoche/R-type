/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Ui
*/

#include "Include/UI.hpp"
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
	maxPlayerLives = 4;
}

void UI::init() {
	Color textColor = WHITE;
	_font = _raylib.loadFont(ASSETS_PATH"/fonts/PressStart2P.ttf");
	Image heartImage = LoadImage(ASSETS_PATH"/sprites/heart.png");
	Image heartEmptyImage = LoadImage(ASSETS_PATH"/sprites/heart_empty.png");
	_fullHeart = LoadTextureFromImage(heartImage);
	_emptyHeart = LoadTextureFromImage(heartEmptyImage);

	UnloadImage(heartImage);
	UnloadImage(heartEmptyImage);

	bool isFrench = (_scene.getGame().getLanguage() == Game::Language::FRENCH);
	game::entities::create_text(
		_reg, 
		BOTTOM_LEFT, 
		{0, 0}, 
		isFrench ? "Joueurs: 0" : "Players: 0", 
		textColor, 
		_spacing, 
		_fontSize, 
		_font
	);
	game::entities::create_text(
		_reg, 
		BOTTOM_CENTER, 
		{0, 0}, 
		isFrench ? "Individuel: 0" : "Individual: 0", 
		textColor, 
		_spacing, 
		_fontSize, 
		_font
	);
	game::entities::create_text(
		_reg, 
		BOTTOM_RIGHT, 
		{0, 0}, 
		isFrench ? "Total: 0" : "Total: 0", 
		textColor, 
		_spacing, 
		_fontSize, 
		_font
	);

	float offsetX = 0;
	for (size_t i = 0; i < maxPlayerLives; ++i) {
		game::entities::create_image(_reg, _fullHeart, TOP_LEFT, Vector2{offsetX, 0});
		offsetX += (_fullHeart.width * _heartScale) + _heartSpacing;
	}
}

void UI::render() {
    auto &dynamic_pos = _reg.get_components<component::dynamic_position>();
    auto &text = _reg.get_components<component::text>();
    auto &types = _reg.get_components<component::type>();
    auto &drawables = _reg.get_components<component::drawable>();
    auto &sprite = _reg.get_components<component::sprite>();

    int playerHealth = 100;
    int maxHealth = 100;
    int playerID = 0;
	int score = 0;
    {
        std::lock_guard<std::mutex> g(_scene._game.getGameClient().stateMutex);
        uint32_t myClientId = _scene._game.getGameClient().clientId;
        
        auto it = _scene._game.getGameClient().playerHealth.find(myClientId);
        if (it != _scene._game.getGameClient().playerHealth.end()) {
            playerHealth = it->second.first;
            maxHealth = it->second.second;
            playerID = myClientId;
        }
    }
    if (playerHealth < 0)
		playerHealth = 0;
    int playerLives = (playerHealth + 24) / 25;
    int currentLive = 1;

    for (std::size_t i = 0; i < dynamic_pos.size() && i < text.size(); ++i) {
        if (!dynamic_pos[i] || !types[i] || !drawables[i]) continue;

        Vector2 offset = {dynamic_pos[i]->offsetX, dynamic_pos[i]->offsetY};
        DynamicPosition dyna_pos = dynamic_pos[i]->position;
        Vector2 pos = getRealPos(_raylib, dyna_pos, offset, Vector2{drawables[i]->width, drawables[i]->height}, _margin);

        switch (types[i]->value) {
            case component::entity_type::TEXT:
                if (text[i]->content.starts_with("Players:"))
                    text[i]->content = std::format("Players: {}", playerID);
                if (text[i]->content.starts_with("Total:")) {
                    int globalScore = 0;
                    {
                        std::lock_guard<std::mutex> g(_scene._game.getGameClient().stateMutex);
                        globalScore = _scene._game.getGameClient().globalScore;
                    }
                    text[i]->content = std::format("Total: {}", globalScore);
                }
                if (text[i]->content.starts_with("Individual:")) {
					uint32_t playerScore = 0;
					{
						std::lock_guard<std::mutex> g(_scene._game.getGameClient().stateMutex);
						auto it = _scene._game.getGameClient().playerIndividualScores.find(_scene._game.getGameClient().clientId);
						if (it != _scene._game.getGameClient().playerIndividualScores.end()) {
							playerScore = it->second;
						}
					}
					text[i]->content = std::format("Individual: {}", playerScore);
				}
                pos = getTextPos(_raylib, dyna_pos, offset, _margin, text[i]->content, _font, _fontSize, _spacing);
                _raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
                break;
            case component::entity_type::IMAGE:
				sprite[i]->texture = _emptyHeart;
				if (currentLive <= playerLives)
					sprite[i]->texture = _fullHeart;
				_raylib.drawTextureEx(sprite[i]->texture, pos, 0, _heartScale, WHITE);
				++currentLive;
				break;
            default:
                break;
        }
    }
    {
        std::string healthText = std::format("HP: {}/{}", playerHealth, maxHealth);
        Vector2 healthPos = {_margin.x, _raylib.getRenderHeight() - _margin.y - 30.f};
        Color healthColor = WHITE;
        
        if (playerHealth <= 25) {
            healthColor = RED;
        } else if (playerHealth <= 50) {
            healthColor = ORANGE;
        }
        
        _raylib.drawTextEx(_font, healthText, healthPos, _fontSize, _spacing, healthColor);
    }
}

void UI::unload() {
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
