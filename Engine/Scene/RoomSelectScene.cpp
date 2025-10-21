/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomSelectScene
*/

#include "Include/RoomSelectScene.hpp"
#include "button.hpp"
#include "components.hpp"
#include "text.hpp"
#include "RenderUtils.hpp"

namespace scene {
	RoomSelectScene::RoomSelectScene(Game &game) : AScene(960, 540, "R-Type - Rooms"), _game(game) {
	}

	void RoomSelectScene::init() {
		_isOpen = true;
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);

		_registry = ecs::registry{};
		_font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");

		_registry.register_component<component::position>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::text>();
		_registry.register_component<component::clickable>();
		_registry.register_component<component::hoverable>();
		_registry.register_component<component::type>();

		_game.getGameClient().sendRoomsFetch();
	}

	void RoomSelectScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);

		_raylib.endDrawing();
	}

	void RoomSelectScene::handleEvents() {
		resetButtonStates();

		switch (_raylib.getKeyPressed()) {
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
			// Button selection
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

			// Button action
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
				if (_selectedButtonIndex == -1 || _selectedButtonIndex >= buttonCount)
					return;
				handleButtonClick(clickable[_selectedButtonIndex + 1]->id);
			}
			if (_selectedButtonIndex != -1 && _selectedButtonIndex < buttonCount)
				hoverable[_selectedButtonIndex + 1]->isHovered = true;
		}
	}

	void RoomSelectScene::drawButton(Vector2 position, Vector2 size, std::string const &content,
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

	void RoomSelectScene::onClose() {
		_raylib.unloadFont(_font);
	}

	void RoomSelectScene::resetButtonStates() {
		auto &positions = _registry.get_components<component::position>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !clickable[i] || !hoverable[i]) continue;
			clickable[i]->isClicked = false;
			hoverable[i]->isHovered = false;
		}
	}

	void RoomSelectScene::handleButtonClick(std::string const &id) {
		if (id == "button_join_room") {
			//TODO: do something
		} else if (id == "button_refresh") {
			//TODO: do something
		} else if (id == "button_create_room") {
			//TODO: do something
		} else if (id == "button_back") {
			_game.getSceneHandler().open("menu");
		} else if (id == "button_quit") {
			close();
		}
	}
}
