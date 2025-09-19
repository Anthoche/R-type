/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#include "MenuScene.hpp"

namespace scene {
	MenuScene::MenuScene(SceneHandler &sceneHandler) : AScene(960, 540, "R-Type - Menu"), _sceneHandler(sceneHandler) {
	}

	void MenuScene::init() {
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);
		_isOpen = true;
	}

	void MenuScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);
		_raylib.drawText("Press E to start", 300, 200, 40, RAYWHITE);
		_raylib.endDrawing();
	}

	void MenuScene::handleEvents() {
		switch (_raylib.getKeyPressed()) {
			case KEY_E:
				/* _sceneHandler.open("game"); */
				break;
			default:
				break;
		}
	}

	void MenuScene::onClose() {
	}
}
