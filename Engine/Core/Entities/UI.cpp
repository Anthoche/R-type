/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Ui
*/

#include "Include/UI.hpp"
#include "text.hpp"

UI::UI(Game &game, ecs::registry &reg, Raylib &raylib) : _game(game), _reg(reg), _raylib(raylib) {
	_margin = Vector2{40, 60};
}

void UI::init() {
	Color textColor = RAYWHITE;
	int fontSize = 20;
	float spacing = -0.5f;
	_font = _raylib.loadFont(ASSETS_PATH"/fonts/PressStart2P.ttf");

	game::entities::create_text(_reg, BOTTOM_LEFT, {0, 0}, "", textColor, spacing, fontSize, _font);
}

void UI::update() {
	auto &dynamic_pos = _reg.get_components<component::dynamic_position>();
	auto &text = _reg.get_components<component::text>();

	for (std::size_t i = 0; i < dynamic_pos.size() && i < text.size(); ++i) {
		if (!dynamic_pos[i]) continue;

		Vector2 offset = {dynamic_pos[i]->offsetX, dynamic_pos[i]->offsetY};
		DynamicPosition dyna_pos = dynamic_pos[i]->position;
		Vector2 pos = getTextPos(dyna_pos, offset);

		_raylib.drawTextEx(text[i]->font, text[i]->content, pos, text[i]->font_size, text[i]->spacing, text[i]->color);
	}
}

void UI::unload() {
}

Vector2 UI::getTextPos(DynamicPosition pos, Vector2 offset) const {
	Vector2 finalPos = {0.f, 0.f};

	switch (pos) {
		case TOP_LEFT:
			break;
		case TOP_RIGHT:
			break;
		case TOP_CENTER:
			break;
		case BOTTOM_LEFT:
			break;
		case BOTTOM_CENTER:
			break;
		case BOTTOM_RIGHT:
			break;
	}
	return finalPos;
}
