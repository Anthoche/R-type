/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SettingsScene
*/

#include "SettingsScene.hpp"

namespace scene {
    SettingsScene::SettingsScene(Game &game) : AScene(960, 540, "R-Type - Settings"), _game(game) {}

    void SettingsScene::init() {
        _raylib.enableCursor();
        _raylib.setTargetFPS(60);
        _isOpen = true;
    }

    void SettingsScene::render() {
        _raylib.beginDrawing();
        _raylib.clearBackground(BLACK);
        _raylib.drawText("Settings Page", 350, 200, 40, RAYWHITE);
        _raylib.drawText("Press ESC to return", 330, 300, 20, LIGHTGRAY);
        _raylib.endDrawing();
    }

    void SettingsScene::handleEvents() {
        if (_raylib.isKeyPressed(KEY_ESCAPE)) {
            _game.getSceneHandler().open("menu");
        }
    }

    void SettingsScene::onClose() {}
}
