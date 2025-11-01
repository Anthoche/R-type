/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Ui
*/


#pragma once

#include "DynamicPosition.hpp"
#include "../../../Engine/Game.hpp"
#include "../../../Engine/Rendering/Raylib.hpp"
#include <vector>

namespace game::scene {
	class GameScene;
}

/**
 * @class UI
 * @brief Manages the user interface for the R-Type game.
 *
 * This class is responsible for rendering and updating all UI elements,
 * including the player's health bar (hearts), scores (individual and global),
 * and player information. It uses the Raylib library for rendering.
 */
class UI {
	public:
		/**
		 * @brief Constructs a UI instance.
		 * @param scene Reference to the GameScene instance.
		 * @param reg Reference to the ECS registry.
		 * @param raylib Reference to the Raylib renderer.
		 */
		UI(game::scene::GameScene &scene, ecs::registry &reg, Raylib &raylib);

		/**
		 * @brief Default destructor.
		 */
		~UI() = default;

		/**
		 * @brief Initializes the UI.
		 */
		void init();

		/**
		 * @brief Renders all UI elements each frame.
		 */
		void render();

		/**
		 * @brief Unloads UI resources.
		 */
		void unload();

		/**
		 * @brief Adds points to the player's individual score.
		 *
		 * @param points Number of points to add to the player's score.
		 */
		void addScore(int points);

		/**
		 * @brief Resets the player's individual score to zero.
		 */
		void resetScore();
		
		private:
		/** @brief Reference to the game scene. */
		game::scene::GameScene &_scene;

		/** @brief Reference to the ECS registry for accessing entities and components. */
		ecs::registry &_reg;

		/** @brief Reference to the Raylib renderer. */
		Raylib &_raylib;

		/** @brief Margin values (x, y) for positioning UI elements on screen. */
		Vector2 _margin;

		/** @brief Font used for rendering text UI elements. */
		Font _font{};

		/** @brief Font size in pixels for UI text. */
		int _fontSize;

		/** @brief Character spacing for text rendering. */
		float _spacing;

		/** @brief Maximum number of lives displayed as hearts. */
		int maxPlayerLives;

		/** @brief Scale factor for heart texture rendering. */
		float _heartScale;

		/** @brief Spacing between individual hearts in pixels. */
		float _heartSpacing;

		/** @brief Texture for a full heart (representing full health). */
		Texture2D _fullHeart{};

		/** @brief Texture for an empty heart (representing lost health). */
		Texture2D _emptyHeart{};

		/** @brief Player's individual score (points earned by this player). */
		int _playerScore = 0;

		/** @brief Vertical spacing between the heart row and the HP text. */
		float _healthTextGap;

		/** @brief Cached heart entities used for rendering life icons. */
		std::vector<ecs::entity_t> _heartEntities;

		/** @brief Rebuilds the heart entities to match the desired number. */
		void rebuildHearts(int desiredLives);
};
