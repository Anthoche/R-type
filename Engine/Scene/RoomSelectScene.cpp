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

		_registry.register_component<component::dynamic_position>();
		_registry.register_component<component::position>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::text>();
		_registry.register_component<component::clickable>();
		_registry.register_component<component::hoverable>();
		_registry.register_component<component::type>();

		refreshRooms();

		game::entities::create_text(_registry, TOP_LEFT, Vector2{0, 0}, "Rooms", RAYWHITE, -0.5f, _titleSize, _font);
		game::entities::create_button(_registry, "button_back", "< Back", BOTTOM_LEFT, Vector2{0, 0},
									{_buttonSize.x - 10, _buttonSize.y}, _accentColor, RAYWHITE, _buttonTextSize);
		game::entities::create_button(_registry, "button_refresh", "Refresh", BOTTOM_RIGHT, Vector2{0, 0},
									{_buttonSize.x + 20, _buttonSize.y}, _accentColor, RAYWHITE, _buttonTextSize);
	}

	void RoomSelectScene::render() {
		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);

		auto &dynamic_pos = _registry.get_components<component::dynamic_position>();
		auto &text = _registry.get_components<component::text>();
		auto &types = _registry.get_components<component::type>();
		auto &drawables = _registry.get_components<component::drawable>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < dynamic_pos.size() && i < text.size(); ++i) {
			if (!dynamic_pos[i] || !types[i] || !drawables[i]) continue;

			Vector2 offset = {dynamic_pos[i]->offsetX, dynamic_pos[i]->offsetY};
			DynamicPosition dyna_pos = dynamic_pos[i]->position;
			Vector2 pos{};
			Vector2 size = {drawables[i]->width, drawables[i]->height};

			if (types[i]->value == component::entity_type::TEXT) {
				pos = getTextPos(_raylib, dyna_pos, offset, _margin, text[i]->content, _font, text[i]->font_size, text[i]->spacing);
				_raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
			}
			if (types[i]->value == component::entity_type::BUTTON) {
				pos = getRealPos(_raylib, dyna_pos, offset, _margin, size);
				std::string content = text[i]->content;
				int fontSize = text[i]->font_size;
				float spacing = text[i]->spacing;
				Color textColor = text[i]->color;
				drawButton(_raylib, pos, size, content, _font, fontSize, spacing, _accentColor, textColor, hoverable[i]->isHovered,
							clickable[i]->isClicked, clickable[i]->enabled);
			}
		}
		_raylib.endDrawing();
	}

	void RoomSelectScene::handleEvents() {
		resetButtonStates();

		switch (_raylib.getKeyPressed()) {
			default:
				break;
		}

		Vector2 mousePos = _raylib.getMousePosition();
		auto &positions = _registry.get_components<component::dynamic_position>();
		auto &drawables = _registry.get_components<component::drawable>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();
		int buttonCount = 0;

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !drawables[i] || !clickable[i] || !hoverable[i])
				continue;

			Vector2 offset = {positions[i]->offsetX, positions[i]->offsetY};
			Vector2 pos = getRealPos(_raylib, positions[i]->position, offset, _margin, _buttonSize);

			if (mousePos.x > pos.x && mousePos.x < pos.x + drawables[i]->width &&
				mousePos.y > pos.y && mousePos.y < pos.y + drawables[i]->height) {
				_selectedButtonIndex = -1;
				hoverable[i]->isHovered = true;
				if (_raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
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

	void RoomSelectScene::onClose() {
		_registry.clear();
		_raylib.unloadFont(_font);
	}

	void RoomSelectScene::refreshRooms() {
		_game.getGameClient().sendRoomsFetch();
		auto &rooms = _game.getGameClient().rooms;
		int i = 0;

		while (rooms.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			i++;
			if (i > 10)
				return;
		}
		for (auto &room : rooms) {
			std::cout << room.second.gameName << std::endl;
		}
	}

	void RoomSelectScene::createRoom(int id, game::serializer::RoomData roomData) {

	}

	void RoomSelectScene::resetButtonStates() {
		auto &positions = _registry.get_components<component::dynamic_position>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
			if (!positions[i] || !clickable[i] || !hoverable[i]) continue;
			clickable[i]->isClicked = false;
			hoverable[i]->isHovered = false;
		}
	}

	void RoomSelectScene::handleButtonClick(std::string const &id) {
		if (id == "button_refresh") {
			refreshRooms();
		} else if (id == "button_create_room") {
			//TODO: do something
		} else if (id == "button_back") {
			_game.getSceneHandler().open("menu");
		}
	}
}
