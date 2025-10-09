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

namespace game::scene {
	class GameScene;
}

class UI {
	public:
		UI(game::scene::GameScene &scene, ecs::registry &reg, Raylib &raylib);
		~UI() = default;

		void init();
		void render();
		void unload();

	private:
		game::scene::GameScene &_scene;
		ecs::registry &_reg;
		Raylib &_raylib;
		Vector2 _margin;
		Font _font{};
		int _fontSize;
		float _spacing;

		int maxPlayerLives;
		float _heartScale;
		float _heartSpacing;
		Texture2D _fullHeart{};
		Texture2D _emptyHeart{};

		Vector2 getTextPos(DynamicPosition pos, Vector2 offset, std::string const &content) const;
		Vector2 getRealPos(DynamicPosition pos, Vector2 offset, Vector2 size) const;
};

#endif //RTYPE_UI_HPP
