/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameScene
*/

#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include "../../Engine/Rendering/scene/AScene.hpp"
#include "../../Engine/Core/Include/entity.hpp"
#include "../Game.hpp"

namespace game::scene {
	/**
	* @class GameScene
	* @brief Represents the main game scene for R-Type.
	*
	* This scene manages the game loop logic, including:
	* - Initialization and rendering.
	* - Handling player input and events.
	* - Managing entities (player, enemies, obstacles).
	* - Setting up systems (movement, rendering, collision, health).
	* - Collision detection and game state updates.
	*/
	class GameScene : public AScene {
	public:
		/**
		* @brief Construct a GameScene with a reference to the SceneHandler.
		* @param game Reference to the game instance.
		*/
		GameScene(Game &game);

		/**
		* @brief Default destructor.
		*/
		~GameScene() override = default;

		// --- Overridden lifecycle methods ---

		/**
		* @brief Initialize the scene (entities, systems, state).
		*/
		void init() override;

		/**
		* @brief Render the scene (draw entities/components).
		*/
		void render() override;

		/**
		* @brief Handle input and system events.
		*/
		void handleEvents() override;

		/**
		* @brief Called when the scene is closed.
		*/
		void onClose() override;

		// --- Event handling ---

		/**
		* @brief Handle player input.
		* @param input_x Horizontal input (movement).
		* @param input_y Vertical input (movement).
		*/
		void handle_input(float input_x, float input_y);

	private:
		// --- Game logic ---

		/**
		* @brief Update the game state (called every frame).
		*/
		void update();

		// --- Game systems ---

		/**
		* @brief Setup the movement system for entities.
		*/
		void setup_movement_system();

		/**
		* @brief Setup the rendering system for entities.
		*/
		void setup_render_system();

		/**
		* @brief Setup the collision detection system.
		*/
		void setup_collision_system();

		/**
		* @brief Setup the health system for entities.
		*/
		void setup_health_system();

		// --- Entity creation ---

		/**
		* @brief Create the player entity and initialize components.
		*/
		void create_player();

		/**
		* @brief Spawn enemy entities.
		*/
		void create_enemies();

		/**
		* @brief Create obstacles in the scene.
		*/
		void create_obstacles();

		// --- Collision management ---

		/**
		* @brief Check and resolve collisions between entities.
		*/
		void check_collisions();

		// --- Entities ---

		ecs::entity_t _player; ///< Main player entity.
		std::vector<ecs::entity_t> _enemies; ///< List of active enemies.
		std::vector<ecs::entity_t> _obstacles; ///< List of active obstacles.

		// --- Game state ---

		bool _game_running; ///< Indicates whether the game is running.
		double _enemy_spawn_timer; ///< Timer tracking enemy spawn intervals.
		double _startTime; ///< Start time of the scene.
		const float _enemy_spawn_interval; ///< Fixed interval between enemy spawns.
		Game &_game;
	};
} // namespace game::scene

#endif // GAMESCENE_HPP
