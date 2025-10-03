/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SettingsScene
*/

#include "SettingsScene.hpp"
#include "components.hpp"
#include "background.hpp"
#include "button.hpp"
#include "text.hpp"

namespace scene {

SettingsScene::SettingsScene(Game &game)
    : AScene(960, 540, "R-Type - Settings"), _game(game) {
    _sceneTitle = "R-Type";

    _buttons = {"1. Difficulty", "2. Lives", "3. Sound"};
    _values = {"Medium", "3", "On"};

    _buttonPosition = {230.f, 200.f};
}

void SettingsScene::init() {
    _isOpen = true;
    _raylib.enableCursor();
    _raylib.setTargetFPS(60);

    _font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");

    _registry.register_component<component::position>();
    _registry.register_component<component::drawable>();
    _registry.register_component<component::text>();
    _registry.register_component<component::clickable>();
    _registry.register_component<component::hoverable>();
    _registry.register_component<component::type>();
	Vector2 titleSize = _raylib.measureTextEx(_font, "R-Type", _titleSize, -0.5f);
	Vector2 titlePos = { (_width - titleSize.x) / 2.0f, 50.0f };

	game::entities::create_text(
    	_registry,
    	titlePos,
    	"R-Type",
    	RAYWHITE,
    	-0.5f,
    	_titleSize,
    	_font
	);


    float y = 200.f;
    for (size_t i = 0; i < _buttons.size(); ++i) {
        game::entities::create_text(
            _registry,
            {_buttonPosition.x, y},
            _buttons[i],
			_accentColor,
            -1.0f,
            _buttonTextSize,
            _font
        );

        game::entities::create_text(
            _registry,
            {_buttonPosition.x + 350.f, y},
            _values[i],
            RAYWHITE,
            -1.0f,
            _buttonTextSize - 4,
            _font
        );

        y += _buttonSize.y + _buttonSpacing;
    }

    Vector2 backPos = {40.f, static_cast<float>(_height - 80)};
    Vector2 backSize = {150.f, 50.f};
    game::entities::create_button(
        _registry, "button_back", "< Back", backPos, backSize, DARKGRAY, RAYWHITE, 23
    );
}

void SettingsScene::render() {
    _raylib.beginDrawing();
    _raylib.clearBackground(BLACK);

    auto &drawables = _registry.get_components<component::drawable>();
    auto &positions = _registry.get_components<component::position>();
    auto &types = _registry.get_components<component::type>();
    auto &text = _registry.get_components<component::text>();
    auto &clickable = _registry.get_components<component::clickable>();
    auto &hoverable = _registry.get_components<component::hoverable>();

    for (std::size_t i = 0; i < positions.size(); ++i) {
        if (!positions[i] || !types[i]) continue;

        Vector2 pos = {positions[i]->x, positions[i]->y};

        if (types[i]->value == component::entity_type::BUTTON) {
            Vector2 size = {drawables[i]->width, drawables[i]->height};
            std::string content = text[i]->content;
            int fontSize = text[i]->font_size;
            float spacing = text[i]->spacing;
            Color textColor = text[i]->color;

            drawButton(
                pos, size, content, fontSize, spacing,
                _accentColor, textColor,
                hoverable[i]->isHovered, clickable[i]->isClicked
            );
        }

        if (types[i]->value == component::entity_type::TEXT) {
            _raylib.drawTextEx(
                text[i]->font,
                text[i]->content,
                pos,
                text[i]->font_size,
                text[i]->spacing,
                text[i]->color
            );
        }
    }

    _raylib.endDrawing();
}

void SettingsScene::handleEvents() {
    resetButtonStates();

    Vector2 mousePos = _raylib.getMousePosition();
    auto &positions = _registry.get_components<component::position>();
    auto &drawables = _registry.get_components<component::drawable>();
    auto &clickable = _registry.get_components<component::clickable>();
    auto &hoverable = _registry.get_components<component::hoverable>();

	for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
		if (!positions[i] || !drawables[i] || !clickable[i] || !hoverable[i]) 
			continue;

        if (mousePos.x > positions[i]->x && mousePos.x < positions[i]->x + drawables[i]->width &&
            mousePos.y > positions[i]->y && mousePos.y < positions[i]->y + drawables[i]->height) {
            hoverable[i]->isHovered = true;
            if (_raylib.isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                clickable[i]->isClicked = true;
                handleButtonClick(clickable[i]->id);
            }
        }
    }
    // if (_raylib.isKeyPressed(KEY_ESCAPE)) {
    //    _game.getSceneHandler().open("menu");
    // }
}

void SettingsScene::handleButtonClick(std::string const &id) {
    if (id == "button_back") {
        _game.getSceneHandler().open("menu");
    }
}

void SettingsScene::drawButton(Vector2 position, Vector2 size, std::string const &content,
                               int fontSize, float spacing, Color color, Color textColor,
                               bool isHovered, bool isClicked) {
    Vector2 textPos = {position.x + size.x / 2, position.y + size.y / 2};
    Vector2 textSize = _raylib.measureTextEx(_font, content, fontSize, spacing);
    textPos.x -= textSize.x / 2;
    textPos.y -= textSize.y / 2;

    Rectangle rect = {position.x, position.y, size.x, size.y};

    if (isHovered) {
        Color temp = textColor;
        textColor = color;
        color = temp;
    }
    if (isClicked) {
        color.a -= 50;
    }

    _raylib.drawRectangleRounded(rect, 0.5, 10, color);
    _raylib.drawTextEx(_font, content, textPos, fontSize, spacing, textColor);
}

void SettingsScene::resetButtonStates() {
    auto &clickable = _registry.get_components<component::clickable>();
    auto &hoverable = _registry.get_components<component::hoverable>();
    auto &positions = _registry.get_components<component::position>();

   for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
        if (!positions[i] || !clickable[i] || !hoverable[i]) 
			continue;
        clickable[i]->isClicked = false;
        hoverable[i]->isHovered = false;
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

}
