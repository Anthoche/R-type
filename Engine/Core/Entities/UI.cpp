/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Ui
*/

#include "Include/UI.hpp"
#include "RenderUtils.hpp"
#include "text.hpp"

UI::UI(Game &game, ecs::registry &reg, Raylib &raylib) : _game(game), _reg(reg), _raylib(raylib) {
	_margin = Vector2{40, 40};
	_fontSize = 18;
	_spacing = -0.5f;
}

void UI::init() {
	Color textColor = RAYWHITE;
	_font = _raylib.loadFont(ASSETS_PATH"/fonts/PressStart2P.ttf");

	game::entities::create_text(_reg, TOP_LEFT, {0, 0}, "206 HDI", textColor, _spacing, _fontSize, _font);
	game::entities::create_text(_reg, TOP_CENTER, {0, 0}, "206 HDI", textColor, _spacing, _fontSize, _font);
	game::entities::create_text(_reg, TOP_RIGHT, {0, 0}, "206 HDI", textColor, _spacing, _fontSize, _font);

	game::entities::create_text(_reg, BOTTOM_LEFT, {0, 0}, "206 HDI", textColor, _spacing, _fontSize, _font);
	game::entities::create_text(_reg, BOTTOM_CENTER, {0, 0}, "206 HDI", textColor, _spacing, _fontSize, _font);
	game::entities::create_text(_reg, BOTTOM_RIGHT, {0, 0}, "206 HDI", textColor, _spacing, _fontSize, _font);
}

void UI::render() {
	auto &dynamic_pos = _reg.get_components<component::dynamic_position>();
	auto &text = _reg.get_components<component::text>();

	for (std::size_t i = 0; i < dynamic_pos.size() && i < text.size(); ++i) {
		if (!dynamic_pos[i] || !text[i]) continue;

		Vector2 offset = {dynamic_pos[i]->offsetX, dynamic_pos[i]->offsetY};
		DynamicPosition dyna_pos = dynamic_pos[i]->position;
		Vector2 pos = getTextPos(dyna_pos, offset, text[i]->content);

		_raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
	}
}

void UI::unload() {
	_raylib.unloadFont(_font);
}

Vector2 UI::getTextPos(DynamicPosition pos, Vector2 offset, std::string const &content) const {
	Vector2 finalPos = {0.f, 0.f};
	Vector2 textSize = _raylib.measureTextEx(_font, content, _fontSize, _spacing);
	float bottomPosY = _raylib.getRenderHeight() - _margin.y - (textSize.y) / 2;
	float centerPosX = getElementCenter(_raylib.getRenderWidth(), textSize.x);
	float rightPosX = _raylib.getRenderWidth() - textSize.x - _margin.x;

	switch (pos) {
		case TOP_LEFT:
			finalPos = {_margin.x, _margin.y};
			break;
		case TOP_CENTER:
			finalPos = {centerPosX, _margin.y};
			break;
		case TOP_RIGHT:
			finalPos = {rightPosX, _margin.y};
			break;
		case BOTTOM_LEFT:
			finalPos = {_margin.x, bottomPosY};
			break;
		case BOTTOM_CENTER:
			finalPos = {centerPosX, bottomPosY};
			break;
		case BOTTOM_RIGHT:
			finalPos = {rightPosX, bottomPosY};
			break;
	}
	finalPos.x += offset.x;
	finalPos.y += offset.y;
	return finalPos;
}
