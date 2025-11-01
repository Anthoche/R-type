/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** MenuScene
*/

#include "Include/MenuScene.hpp"
#include "button.hpp"
#include "components.hpp"
#include "text.hpp"
#include "RenderUtils.hpp"

namespace scene {
	MenuScene::MenuScene(Game &game) : AScene(960, 540, "R-Type - Menu"), _game(game) {
		_titleCenterY = getElementCenter(_height, _titleSize);
		_buttonCenterY = static_cast<float>(getButtonsCenterY(_height, 3, _buttonSize.y, _buttonSpacing));
		_buttonPosition = {600.f, _buttonCenterY};
	}

	void MenuScene::init() {
		_registry.clear();
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

		_buttonPosition = {600.f, _buttonCenterY};

		game::entities::create_text(_registry, {20.0f, _titleCenterY}, "R-Type", RAYWHITE, -0.5f, _titleSize, _font);

		bool isFrench = (_game.getLanguage() == Game::Language::FRENCH);
		bool isItalian = (_game.getLanguage() == Game::Language::ITALIAN);

		game::entities::create_button(_registry, "button_play", isFrench ? "Jouer" : isItalian ? "Gioca" : "Play",
			_buttonPosition.x, _buttonPosition.y, 0.f, _buttonSize.x, _buttonSize.y, _accentColor, RAYWHITE);

		_buttonPosition.y += _buttonSize.y + _buttonSpacing;

		game::entities::create_button(_registry, "button_settings", isFrench ? "Options" : isItalian ? "Opzioni" : "Settings",
			_buttonPosition.x, _buttonPosition.y, 0.f, _buttonSize.x, _buttonSize.y, _accentColor, RAYWHITE);

		_buttonPosition.y += _buttonSize.y + _buttonSpacing;

		game::entities::create_button(_registry, "button_quit", isFrench ? "Quitter" : isItalian ? "Uscire" : "Quit",
			_buttonPosition.x, _buttonPosition.y, 0.f, _buttonSize.x, _buttonSize.y, _accentColor, RAYWHITE);

		if (!_game.getGameClient().isConnected())
			_game.getGameClient().sendHello();
	}


	void MenuScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);

		auto &drawables = _registry.get_components<component::drawable>();
		auto &positions = _registry.get_components<component::position>();
		auto &types = _registry.get_components<component::type>();
		auto &text = _registry.get_components<component::text>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
			if (!positions[i] || !drawables[i] || !types[i]) continue;

			Vector2 pos = {positions[i]->x, positions[i]->y};
			Vector2 size = {drawables[i]->width, drawables[i]->height};

			if (types[i]->value == component::entity_type::BUTTON) {
				std::string content = text[i]->content;
				int fontSize = text[i]->font_size;
				float spacing = text[i]->spacing;
				Color textColor = text[i]->color;
				drawButton(pos, size, content, fontSize, spacing, _accentColor, textColor, hoverable[i]->isHovered, clickable[i]->isClicked);
			}
			if (types[i]->value == component::entity_type::TEXT) {
				_raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
			}
		}
		_raylib.endDrawing();
	}

	void MenuScene::handleEvents() {
		resetButtonStates();

		switch (_raylib.getKeyPressed()) {
			case KEY_S:
				_game.getSceneHandler().open("settings");
				break;
			default:
				break;
		}

		Vector2 mousePos = _raylib.getMousePosition();
		auto &positions = _registry.get_components<component::position>();
		auto &drawables = _registry.get_components<component::drawable>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();
		int buttonCount = 0;

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !drawables[i] || !clickable[i] || !hoverable[i]) 
				continue;
			if (mousePos.x > positions[i]->x && mousePos.x < positions[i]->x + drawables[i]->width &&
				mousePos.y > positions[i]->y && mousePos.y < positions[i]->y + drawables[i]->height) {
				_selectedButtonIndex = -1;
				hoverable[i]->isHovered = true;
				if (_raylib.isMouseButtonDown(MOUSE_BUTTON_LEFT)) {
					clickable[i]->isClicked = true;
					handleButtonClick(clickable[i]->id);
				}
			}
			buttonCount++;
		}

		if (_raylib.isGamepadAvailable(0)) {
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
				if (_selectedButtonIndex >= buttonCount - 1) {
					_selectedButtonIndex = 0;
					return;
				}
				_selectedButtonIndex++;
			}
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
				if (_selectedButtonIndex <= 0) {
					_selectedButtonIndex = buttonCount - 1;
					return;
				}
				_selectedButtonIndex--;
			}

			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
				if (_selectedButtonIndex == -1 || _selectedButtonIndex >= buttonCount)
					return;
				handleButtonClick(clickable[_selectedButtonIndex + 1]->id);
			}
			if (_selectedButtonIndex != -1 && _selectedButtonIndex < buttonCount)
				hoverable[_selectedButtonIndex + 1]->isHovered = true;
		}
	}

	void MenuScene::drawButton(Vector2 position, Vector2 size, std::string const &content,
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

	void MenuScene::onClose() {
		_raylib.unloadFont(_font);
	}

	void MenuScene::resetButtonStates() {
		auto &positions = _registry.get_components<component::position>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !clickable[i] || !hoverable[i]) continue;
			clickable[i]->isClicked = false;
			hoverable[i]->isHovered = false;
		}
	}

	void MenuScene::handleButtonClick(std::string const &id) {
		if (id == "button_play") {
			_game.getSceneHandler().open("room_select");
		} else if (id == "button_settings") {
			 _game.getSceneHandler().open("settings");
		} else if (id == "button_quit") {
			close();
		}
	}

}
