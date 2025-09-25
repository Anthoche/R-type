/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#include "MenuScene.hpp"
#include "components.hpp"

namespace scene {
	MenuScene::MenuScene(Game &game) : AScene(960, 540, "R-Type - Menu"), _game(game) {
		_gameTitle = "R-Type";
		_titleCenterY = static_cast<float>(getCenterY(_height, _titleSize));
		_buttons = {"Play", "Settings", "Quit"};

		_buttonCenterY = static_cast<float>(getButtonsCenterY(_height, _buttons.size(), _buttonSize.y, _buttonSpacing));
		_buttonPosition = {600.f, _buttonCenterY};
	}

	void MenuScene::init() {
		_isOpen = true;
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);

		_font = _raylib.loadFont(ASSETS_PATH"/fonts/PressStart2P.ttf");

		_game.getGameClient().sendHello();
	}

	void MenuScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);

		_raylib.drawTextEx(_font, _gameTitle, (Vector2){20.0f, _titleCenterY}, _titleSize, -0.5f, WHITE);

		float buttonPosY = _buttonPosition.y;
		for (auto &button: _buttons) {
			Vector2 textPos = {_buttonPosition.x + _buttonSize.x / 2, buttonPosY + _buttonSize.y / 2};
			Vector2 textSize = _raylib.measureTextEx(_font, button, _buttonTextSize, -1);
			textPos.x -= textSize.x / 2;
			textPos.y -= textSize.y / 2;

			Rectangle rect = {_buttonPosition.x, buttonPosY, _buttonSize.x, _buttonSize.y};
			_raylib.drawRectangleRounded(rect, 0.5, 10, _accentColor);
		 	_raylib.drawTextEx(_font, button, textPos, _buttonTextSize, -1, WHITE);
			buttonPosY += _buttonSize.y + _buttonSpacing;
		}
		_raylib.endDrawing();
	}

	void MenuScene::handleEvents() {
		if (_game.getGameStatus() == GameStatus::RUNNING) {
			_game.getSceneHandler().open("game");
			return;
		}

		switch (_raylib.getKeyPressed()) {
			default:
				break;
		}
	}

	void MenuScene::onClose() {
		_raylib.unloadFont(_font);
	}

	int MenuScene::getCenterY(int screenHeight, int elementHeight) const {
		return (screenHeight / 2) - (elementHeight / 2);
	}

	int MenuScene::getButtonsCenterY(int screenHeight, int numberOfButtons, int buttonHeight, int buttonSpacing) const {
		int totalHeight = (numberOfButtons * buttonHeight) + ((numberOfButtons - 1) * buttonSpacing);
		return getCenterY(screenHeight, totalHeight);
	}
}
