/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomCreationScene (Smash variant)
*/

#include "Include/RoomCreationScene.hpp"
#include "button.hpp"
#include "components.hpp"
#include "text.hpp"

#include <algorithm>

namespace scene {

RoomCreationScene::RoomCreationScene(Game &game)
	: AScene(960, 540, "R-Type - Create Room"), _game(game) {}

void RoomCreationScene::init() {
	_registry.clear();
	_isOpen = true;
	_raylib.enableCursor();
	_raylib.setTargetFPS(60);

	_font = _raylib.loadFont(ASSETS_PATH "/fonts/PressStart2P.ttf");

	registerComponents();
	createLayout();
}

void RoomCreationScene::registerComponents() {
	_registry.register_component<component::position>();
	_registry.register_component<component::drawable>();
	_registry.register_component<component::text>();
	_registry.register_component<component::clickable>();
	_registry.register_component<component::hoverable>();
	_registry.register_component<component::type>();
}

void RoomCreationScene::createLayout() {
	std::string title = "Create Room";
	Vector2 titleSize = _raylib.measureTextEx(_font, title.c_str(), 54, -0.5f);
	Vector2 titlePos = {(_width - titleSize.x) / 2.f, 60.f};
	game::entities::create_text(_registry, titlePos, title, RAYWHITE, -0.5f, 54, _font);

	std::string helper = "Set the minimum and maximum players, then confirm.";
	Vector2 helperSize = _raylib.measureTextEx(_font, helper.c_str(), 18, -0.5f);
	Vector2 helperPos = {(_width - helperSize.x) / 2.f, titlePos.y + 70.f};
	game::entities::create_text(_registry, helperPos, helper, GRAY, -0.5f, 18, _font);

	createValueRow("Min players", 220.f, "button_min_decrease", "button_min_increase", _minValueButtonId, _minPlayers);
	createValueRow("Max players", 300.f, "button_max_decrease", "button_max_increase", _maxValueButtonId, _maxPlayers);

	float createWidth = 260.f;
	float createX = (_width - createWidth) / 2.f;
	game::entities::create_button(
		_registry,
		"button_create_room_confirm",
		"Create room",
		createX,
		380.f,
		0.f,
		createWidth,
		55.f,
		_accentColor,
		BLACK,
		26
	);

	game::entities::create_button(
		_registry,
		"button_back",
		"< Back",
		40.f,
		static_cast<float>(_height - 80),
		0.f,
		150.f,
		50.f,
		_accentColor,
		BLACK,
		23
	);
}

void RoomCreationScene::createValueRow(const std::string &label, float y, const std::string &minusId,
									   const std::string &plusId, const std::string &valueId, int value) {
	game::entities::create_text(_registry, {120.f, y + 10.f}, label, RAYWHITE, -0.5f, 24, _font);

	float buttonWidth = 55.f;
	float buttonHeight = 50.f;
	float spacing = 18.f;
	float valueWidth = 140.f;
	float valueX = (_width - valueWidth) / 2.f;

	game::entities::create_button(
		_registry,
		minusId,
		"<",
		valueX - buttonWidth - spacing,
		y,
		0.f,
		buttonWidth,
		buttonHeight,
		_accentColor,
		BLACK,
		32
	);

	auto valueEntity = game::entities::create_button(
		_registry,
		valueId,
		std::to_string(value),
		valueX,
		y,
		0.f,
		valueWidth,
		buttonHeight,
		DARKGRAY,
		RAYWHITE,
		28
	);

	auto &clickables = _registry.get_components<component::clickable>();
	if (valueEntity < clickables.size() && clickables[valueEntity])
		clickables[valueEntity]->enabled = false;

	game::entities::create_button(
		_registry,
		plusId,
		">",
		valueX + valueWidth + spacing,
		y,
		0.f,
		buttonWidth,
		buttonHeight,
		_accentColor,
		BLACK,
		32
	);
}

void RoomCreationScene::render() {
	_raylib.beginDrawing();
	_raylib.clearBackground(BLACK);

	auto &drawables = _registry.get_components<component::drawable>();
	auto &positions = _registry.get_components<component::position>();
	auto &types = _registry.get_components<component::type>();
	auto &text = _registry.get_components<component::text>();
	auto &clickable = _registry.get_components<component::clickable>();
	auto &hoverable = _registry.get_components<component::hoverable>();

	for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < types.size(); ++i) {
		if (!positions[i] || !types[i])
			continue;

		Vector2 pos = {positions[i]->x, positions[i]->y};
		if (types[i]->value == component::entity_type::BUTTON && drawables[i] && text[i]) {
			Vector2 size = {drawables[i]->width, drawables[i]->height};
			Color bg = drawables[i]->color;
			Color fg = text[i]->color;
			bool hovered = (hoverable.size() > i && hoverable[i]) ? hoverable[i]->isHovered : false;
			bool clicked = (clickable.size() > i && clickable[i]) ? clickable[i]->isClicked : false;

			Vector2 textSize = _raylib.measureTextEx(_font, text[i]->content, text[i]->font_size, text[i]->spacing);
			Vector2 textPos = {pos.x + (size.x - textSize.x) / 2.f, pos.y + (size.y - textSize.y) / 2.f};

			Color drawBg = bg;
			Color drawFg = fg;
			if (hovered && clickable.size() > i && clickable[i] && clickable[i]->enabled) {
				std::swap(drawBg, drawFg);
			}
			if (clicked)
				drawBg.a = std::max(0, drawBg.a - 50);

			_raylib.drawRectangleRounded({pos.x, pos.y, size.x, size.y}, 0.5f, 10, drawBg);
			_raylib.drawTextEx(_font, text[i]->content, textPos, text[i]->font_size, text[i]->spacing, drawFg);
		} else if (types[i]->value == component::entity_type::TEXT && text[i]) {
			_raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
		}
	}

	_raylib.endDrawing();
}

void RoomCreationScene::handleEvents() {
	resetButtonStates();

	Vector2 mousePos = _raylib.getMousePosition();
	auto &positions = _registry.get_components<component::position>();
	auto &drawables = _registry.get_components<component::drawable>();
	auto &clickable = _registry.get_components<component::clickable>();
	auto &hoverable = _registry.get_components<component::hoverable>();

	for (std::size_t i = 0; i < positions.size() && i < drawables.size() && i < clickable.size() && i < hoverable.size(); ++i) {
		if (!positions[i] || !drawables[i] || !clickable[i] || !hoverable[i])
			continue;
		if (!clickable[i]->enabled)
			continue;

		Vector2 size = {drawables[i]->width, drawables[i]->height};
		if (mousePos.x > positions[i]->x && mousePos.x < positions[i]->x + size.x &&
			mousePos.y > positions[i]->y && mousePos.y < positions[i]->y + size.y) {
			hoverable[i]->isHovered = true;
			if (_raylib.isMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
				clickable[i]->isClicked = true;
				handleButtonClick(clickable[i]->id);
			}
		}
	}
}

void RoomCreationScene::handleButtonClick(const std::string &id) {
	if (id == "button_back") {
		_game.getSceneHandler().open("room_select");
		return;
	}
	if (id == "button_create_room_confirm") {
		submitRoom();
		return;
	}
	if (id == "button_min_decrease")
		changeMin(-1);
	else if (id == "button_min_increase")
		changeMin(1);
	else if (id == "button_max_decrease")
		changeMax(-1);
	else if (id == "button_max_increase")
		changeMax(1);
}

void RoomCreationScene::changeMin(int delta) {
	_minPlayers = std::clamp(_minPlayers + delta, _minLimit, _maxPlayers);
	if (_minPlayers > _maxPlayers)
		_maxPlayers = _minPlayers;
	updateValueDisplay(_minValueButtonId, _minPlayers);
	updateValueDisplay(_maxValueButtonId, _maxPlayers);
}

void RoomCreationScene::changeMax(int delta) {
	_maxPlayers = std::clamp(_maxPlayers + delta, _minPlayers, _maxLimit);
	updateValueDisplay(_maxValueButtonId, _maxPlayers);
}

void RoomCreationScene::updateValueDisplay(const std::string &buttonId, int value) {
	auto &clickable = _registry.get_components<component::clickable>();
	auto &texts = _registry.get_components<component::text>();

	for (std::size_t i = 0; i < clickable.size() && i < texts.size(); ++i) {
		if (clickable[i] && clickable[i]->id == buttonId && texts[i]) {
			texts[i]->content = std::to_string(value);
			break;
		}
	}
}

void RoomCreationScene::submitRoom() {
	_game.getGameClient().sendRoomCreate(static_cast<uint16_t>(_minPlayers), static_cast<uint16_t>(_maxPlayers));
	_game.getSceneHandler().open("room_select");
}

void RoomCreationScene::resetButtonStates() {
	auto &positions = _registry.get_components<component::position>();
	auto &clickable = _registry.get_components<component::clickable>();
	auto &hoverable = _registry.get_components<component::hoverable>();

	for (std::size_t i = 0; i < positions.size() && i < clickable.size() && i < hoverable.size(); ++i) {
		if (!positions[i] || !clickable[i] || !hoverable[i])
			continue;
		clickable[i]->isClicked = false;
		hoverable[i]->isHovered = false;
	}
}

void RoomCreationScene::onClose() {
	_raylib.unloadFont(_font);
}

} // namespace scene
