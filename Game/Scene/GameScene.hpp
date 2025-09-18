/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameScene
*/

#ifndef GAMESCENE_HPP
	#define GAMESCENE_HPP

	#include "../../Engine/Rendering/scene/AScene.hpp"
	#include "../../Engine/Rendering/scene/SceneHandler.hpp"
	#include "../../Engine/Core/Include/entity.hpp"
	#include "../Entities/Include/components.hpp"

namespace game::scene {
	class GameScene : public AScene {
	public:
		GameScene(SceneHandler &sceneHandler);
		~GameScene() override = default;

		GameScene();

		// Overriding methods
		void init() override;
		void render() override;
		void handleEvents() override;
		void onClose() override;

		// Gestion des événements
		void handle_input(float input_x, float input_y);

	private:
		// Mise à jour de la logique de jeu
		void update();

		// Systèmes de jeu
		void setup_movement_system();

		void setup_render_system();

		void setup_collision_system();

		void setup_health_system();

		// Création d'entités
		void create_player();

		void create_enemies();

		void create_obstacles();

		// Gestion des collisions
		void check_collisions();

		// Entités principales
		ecs::entity_t _player;
		std::vector<ecs::entity_t> _enemies;
		std::vector<ecs::entity_t> _obstacles;

		SceneHandler &_sceneHandler;

		// État du jeu
		bool _game_running;
		double _enemy_spawn_timer;
		double _startTime;
		const float _enemy_spawn_interval;
	};
} // namespace scene

#endif //GAMESCENE_HPP