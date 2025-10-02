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

        _raylib.drawText("R-Type", 380, 50, 60, RAYWHITE);

        _raylib.drawText("1. Difficulty", 250, 180, 30, WHITE);
        _raylib.drawText("Medium", 600, 180, 30, RAYWHITE);

        _raylib.drawText("2. Lives", 250, 250, 30, WHITE);
        _raylib.drawText("3", 600, 250, 30, RAYWHITE);

        _raylib.drawText("3. Sound", 250, 320, 30, WHITE);
        _raylib.drawText("On", 600, 320, 30, RAYWHITE);

        _raylib.drawText("< Back", 50, 480, 25, LIGHTGRAY);

        _raylib.endDrawing();
    }

    void SettingsScene::handleEvents() {
        if (_raylib.isKeyPressed(KEY_ESCAPE)) {
            _game.getSceneHandler().open("menu");
        }
    }

    void SettingsScene::onClose() {}
}
