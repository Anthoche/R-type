/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SettingsScene
*/

#include "SettingsScene.hpp"
#include "components.hpp"

namespace scene {

SettingsScene::SettingsScene(Game &game) : AScene(960, 540, "R-Type - Settings"), _game(game) {
    _sceneTitle = "R-Type";

    _buttons = {"1. Difficulty", "2. Lives", "3. Sound"};

    _values = {"Medium", "3", "On"};

    _buttonPosition = {280.f, 200.f};
}

void SettingsScene::init() {
    _isOpen = true;
    _raylib.enableCursor();
    _raylib.setTargetFPS(60);

    _font = _raylib.loadFont(ASSETS_PATH"/fonts/PressStart2P.ttf");
}

void SettingsScene::render() {
    _raylib.beginDrawing();
    _raylib.clearBackground(BLACK);

    Vector2 titleSize = _raylib.measureTextEx(_font, _sceneTitle, _titleSize, -0.5f);
    Vector2 titlePos = {(float)(_width / 2 - titleSize.x / 2), 50.f};
    _raylib.drawTextEx(_font, _sceneTitle, titlePos, _titleSize, -0.5f, WHITE);

    float leftMargin = _width / 4.f;
    _buttonPosition.x = leftMargin;

    float maxTextWidth = 0.f;
    for (auto &button : _buttons) {
        Vector2 textSize = _raylib.measureTextEx(_font, button, _buttonTextSize, -1);
        if (textSize.x > maxTextWidth)
            maxTextWidth = textSize.x;
    }
	
    float buttonWidth = maxTextWidth + 40.f;
	float buttonHeight = _buttonSize.y;

    float buttonPosY = _buttonPosition.y;
    for (size_t i = 0; i < _buttons.size(); ++i) {
        Rectangle rect = {_buttonPosition.x, buttonPosY, buttonWidth, buttonHeight};
        _raylib.drawRectangleRounded(rect, 0.5, 10, _accentColor);

        Vector2 textSize = _raylib.measureTextEx(_font, _buttons[i], _buttonTextSize, -1);
        Vector2 textPos = {_buttonPosition.x + 20.f, buttonPosY + buttonHeight / 2 - textSize.y / 2};
        _raylib.drawTextEx(_font, _buttons[i], textPos, _buttonTextSize, -1, WHITE);

        Vector2 valueTextSize = _raylib.measureTextEx(_font, _values[i], _buttonTextSize - 4, -1);
        Vector2 valuePos = {_buttonPosition.x + buttonWidth + 15.f,
                            buttonPosY + buttonHeight / 2 - valueTextSize.y / 2};
        _raylib.drawTextEx(_font, _values[i], valuePos, _buttonTextSize - 4, -1, WHITE);

        buttonPosY += buttonHeight + _buttonSpacing;
    }

    std::string backText = "< Back";
    float backMargin = 40;
    Vector2 backPos = {backMargin, _height - _buttonTextSize - backMargin};
    _raylib.drawTextEx(_font, backText, backPos, _buttonTextSize, -1, WHITE);

    _raylib.endDrawing();
}



void SettingsScene::handleEvents() {
    if (_raylib.isKeyPressed(KEY_ESCAPE) || _raylib.isKeyPressed(KEY_BACKSPACE)) {
        _game.getSceneHandler().open("menu");
    }
}


void SettingsScene::onClose() {
    _raylib.unloadFont(_font);
}


int SettingsScene::getCenterY(int screenHeight, int elementHeight) const {
    return (screenHeight / 2) - (elementHeight / 2);
}


int SettingsScene::getButtonsCenterY(int screenHeight, int numberOfButtons, int buttonHeight, int buttonSpacing) const {
    int totalHeight = (numberOfButtons * buttonHeight) + ((numberOfButtons - 1) * buttonSpacing);
    return getCenterY(screenHeight, totalHeight);
}

} // namespace scene
