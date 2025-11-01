/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomSelectScene
*/

#include "Include/RoomSelectScene.hpp"
#include "button.hpp"
#include "components.hpp"
#include "Logger.hpp"
#include "text.hpp"
#include "RenderUtils.hpp"
#include <chrono>

namespace scene {
	RoomSelectScene::RoomSelectScene(Game &game) : AScene(960, 540, "R-Type - Rooms"), _game(game) {
		_currentRoomPosY = _baseRoomPosition.y;
	}

	void RoomSelectScene::init() {
		_registry.clear();
		_isOpen = true;
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);

		_font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");

		_registry.register_component<component::dynamic_position>();
		_registry.register_component<component::position>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::text>();
		_registry.register_component<component::clickable>();
		_registry.register_component<component::hoverable>();
		_registry.register_component<component::type>();

		refreshRooms();

		Vector2 titleSize = _raylib.measureTextEx(_font, "Rooms", static_cast<float>(_titleSize), -0.5f);
		Vector2 titlePos = {(_width - titleSize.x) / 2.f, 40.f};
		game::entities::create_text(_registry, titlePos, "Rooms", RAYWHITE, -0.5f, _titleSize, _font);
		game::entities::create_button(_registry, "button_back", "< Back", BOTTOM_LEFT, Vector2{0, 0},
									{_buttonSize.x - 10, _buttonSize.y}, _accentColor, BLACK, _buttonTextSize);
		game::entities::create_button(_registry, "button_refresh", "Refresh", BOTTOM_RIGHT, Vector2{0, 0},
									{_buttonSize.x + 20, _buttonSize.y}, _accentColor, BLACK, _buttonTextSize);
	}

	void RoomSelectScene::render() {
		_currentRoomPosY = _baseRoomPosition.y;

		auto &dynamic_pos = _registry.get_components<component::dynamic_position>();
		auto &text = _registry.get_components<component::text>();
		auto &types = _registry.get_components<component::type>();
		auto &drawables = _registry.get_components<component::drawable>();
		auto &clickable = _registry.get_components<component::clickable>();
		auto &hoverable = _registry.get_components<component::hoverable>();

		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);

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
				drawButton(_raylib, pos, size, content, _font, fontSize, spacing, drawables[i]->color, textColor, hoverable[i]->isHovered,
							clickable[i]->isClicked, clickable[i]->enabled);
			}
		}
		displayRooms();
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
			Vector2 size = {drawables[i]->width, drawables[i]->height};
			Vector2 pos = getRealPos(_raylib, positions[i]->position, offset, _margin, size);

			if (mousePos.x > pos.x && mousePos.x < pos.x + size.x &&
				mousePos.y > pos.y && mousePos.y < pos.y + size.y) {
				_selectedButtonIndex = -1;
				hoverable[i]->isHovered = true;
				if (_raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					clickable[i]->isClicked = true;
					handleButtonClick(clickable[i]->id);
				}
				break;
			}
			buttonCount++;
		}

		for (auto &room: _rooms) {
			Vector2 pos = {room.second.button.rect.x, room.second.button.rect.y};
			Vector2 size = {room.second.button.rect.width, room.second.button.rect.height};

			if (mousePos.x > pos.x && mousePos.x < pos.x + size.x &&
				mousePos.y > pos.y && mousePos.y < pos.y + size.y) {
				_selectedButtonIndex = -1;
				room.second.button.isHovered = true;
				if (_raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
					room.second.button.isClicked = true;
					handleRoomJoinButton(room.first);
				}
				break;
			}
		}

		if (_raylib.isGamepadAvailable(0)) {
			// Button selection
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) {
				_selectingRooms = false;
				_selectedRoomIndex = -1;
				if (_selectedButtonIndex >= buttonCount - 1) {
					_selectedButtonIndex = 0;
					return;
				}
				_selectedButtonIndex++;
			}
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT)) {
				_selectingRooms = false;
				_selectedRoomIndex = -1;
				if (_selectedButtonIndex <= 0) {
					_selectedButtonIndex = buttonCount - 1;
					return;
				}
				_selectedButtonIndex--;
			}

			//Room selection
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN)) {
				_selectedButtonIndex = -1;
				_selectingRooms = true;
				if (_selectedRoomIndex >= _rooms.size() - 1) {
					_selectedRoomIndex = 0;
					return;
				}
				_selectedRoomIndex++;
			}
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP)) {
				_selectedButtonIndex = -1;
				_selectingRooms = true;
				if (_selectedRoomIndex <= 0) {
					_selectedRoomIndex = _rooms.size() - 1;
					return;
				}
				_selectedRoomIndex--;
			}

			// Button action
			if (_raylib.isGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) {
				if (_selectingRooms) {
					if (_selectedRoomIndex == -1 || _selectedRoomIndex >= _rooms.size())
						return;
					handleRoomJoinButton(_selectedRoomIndex);
				} else {
					if (_selectedButtonIndex == -1 || _selectedButtonIndex >= buttonCount)
						return;
					handleButtonClick(clickable[_selectedButtonIndex + 1]->id);
				}
			}
			if (_selectingRooms) {
				if (_selectedRoomIndex != -1 && _selectedRoomIndex < _rooms.size())
					_rooms[_selectedRoomIndex].button.isHovered = true;
			} else {
				if (_selectedButtonIndex != -1 && _selectedButtonIndex < buttonCount)
					hoverable[_selectedButtonIndex + 1]->isHovered = true;
			}
		}
	}

	void RoomSelectScene::onClose() {
		_registry.clear();
		_raylib.unloadFont(_font);
	}

	void RoomSelectScene::refreshRooms() {
		_game.getGameClient().sendRoomsFetch();
		_rooms.clear();
		std::map<int, game::serializer::RoomData> snapshot;

		if (!_game.getGameClient().waitForRooms(std::chrono::milliseconds(1500), snapshot)) {
			LOG_ERROR("Timed out while trying to retrieve rooms data.");
			return;
		}
		for (auto &room: snapshot) {
			createRoom(room.first, room.second);
		}
	}

	void RoomSelectScene::displayRooms() {
		for (auto &room: _rooms) {
			room.second.background.x = getElementCenter(_raylib.getRenderWidth(), _roomSize.x);

			Vector2 pos = {room.second.background.x, room.second.background.y};
			Vector2 nameSize = _raylib.measureTextEx(_font, room.second.name, _roomNameSize, 0.75);
			Vector2 playerSize = _raylib.measureTextEx(_font, room.second.playersCount, _playerCountSize, 0.75);
			Vector2 namePos = {pos.x + _innerMargin, pos.y + getElementCenter(_roomSize.y, nameSize.y)};
			Vector2 playersPos = {pos.x + _roomSize.x - _innerMargin - 150, pos.y + getElementCenter(_roomSize.y, playerSize.y)};
			Vector2 buttonPos = {
				pos.x + room.second.background.width - _innerMargin - room.second.button.rect.width,
				pos.y + getElementCenter(_roomSize.y, _roomJoinButtonSize.y)
			};

			room.second.button.rect.x = buttonPos.x;
			room.second.button.rect.y = buttonPos.y;
			_raylib.drawRectangleRounded(room.second.background, 0.5, 10, _roomBackgroundColor);
			drawButton(_raylib, buttonPos, _roomJoinButtonSize, "Join", _font, _roomButtonTextSize, 0.75f, _accentColor, BLACK,
						room.second.button.isHovered, room.second.button.isClicked, true, 0.75f);
			_raylib.drawTextEx(_font, room.second.name, namePos, _roomNameSize, 0.75f, RAYWHITE);
			_raylib.drawTextEx(_font, room.second.playersCount, playersPos, _playerCountSize, 0.75, RAYWHITE);
		}
	}

	void RoomSelectScene::createRoom(uint32_t id, game::serializer::RoomData roomData) {
		Vector2 pos = {_baseRoomPosition.x, static_cast<float>(_currentRoomPosY)};
		RoomDisplay room{};

		room.background = Rectangle{pos.x, pos.y, _roomSize.x, _roomSize.y};
		room.name = roomData.gameName;
		room.playersCount = std::format("{}/{}", roomData.currentPlayers, roomData.maxPlayers);
		room.button = RoomButton{Rectangle{pos.x, pos.y, _roomJoinButtonSize.x, _roomJoinButtonSize.y}, false, false};
		_currentRoomPosY += _roomSize.y + _roomSpacing;
		_rooms.insert_or_assign(id, room);
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

		for (auto &room: _rooms) {
			room.second.button.isHovered = false;
			room.second.button.isClicked = false;
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

	void RoomSelectScene::handleRoomJoinButton(uint32_t id) {
		_game.getGameClient().sendRoomAsk(id);
		_game.getSceneHandler().open("waiting");
	}
}
