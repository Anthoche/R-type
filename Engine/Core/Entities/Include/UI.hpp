/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Ui
*/

#ifndef RTYPE_UI_HPP
	#define RTYPE_UI_HPP

#include "DynamicPosition.hpp"
#include "../Game.hpp"
#include "registry.hpp"
#include "Raylib.hpp"

class UI {
	public:
		UI(Game &game, ecs::registry &reg, Raylib &raylib);
		~UI() = default;

		void init();
		void update();
		void unload();

	private:
		Game &_game;
		ecs::registry &_reg;
		Raylib &_raylib;
		Vector2 _margin;
		Font _font{};

		Vector2 getTextPos(DynamicPosition pos, Vector2 offset) const;
};

#endif //RTYPE_UI_HPP
