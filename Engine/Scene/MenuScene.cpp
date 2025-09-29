/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#include "Include/MenuScene.hpp"

namespace scene {
	MenuScene::MenuScene(Game &game) : AScene(960, 540, "R-Type - Menu"), _game(game) {
	}

	void MenuScene::init() {
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);
		_isOpen = true;

		_game.getGameClient().sendHello();
	}

	void MenuScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);
		_raylib.drawText("Waiting for players...", 300, 200, 40, RAYWHITE);
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
	}
}
