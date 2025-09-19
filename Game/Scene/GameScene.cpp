#include "GameScene.hpp"
#include "../Entities/Include/player.hpp"
#include <random>
#include <algorithm>
#include <iostream>

namespace game::scene {
	GameScene::GameScene(SceneHandler &sceneHandler) : AScene(800, 600, "R-Type"), _player(ecs::entity_t{0}), _sceneHandler(sceneHandler),
														_enemy_spawn_interval(2.0f) {
		_raylib = Raylib();
		_game_running = true;
		_enemy_spawn_timer = 0.f;
		_startTime = 0.f;
	}

	void GameScene::init() {
		_isOpen = true;
		_startTime = _raylib.getTime();
		_raylib.enableCursor();
		_raylib.setTargetFPS(60);

		_registry.register_component<component::position>();
		_registry.register_component<component::velocity>();
		_registry.register_component<component::drawable>();
		_registry.register_component<component::controllable>();
		_registry.register_component<component::health>();
		_registry.register_component<component::damage>();
		_registry.register_component<component::type>();
		_registry.register_component<component::collision_box>();

		setup_movement_system();
		setup_render_system();
		setup_collision_system();
		setup_health_system();

		game::entities::setup_player_control_system(_registry);
		game::entities::setup_player_bounds_system(_registry, static_cast<float>(_width), static_cast<float>(_height));

		create_player();
		create_obstacles();
	}

	void GameScene::update() {
		if (!_game_running) return;

		double delta_time = _startTime - _raylib.getTime();
		_enemy_spawn_timer += delta_time;
		if (_enemy_spawn_timer >= _enemy_spawn_interval) {
			create_enemies();
			_enemy_spawn_timer = 0.f;
		}

		_registry.run_systems();

		check_collisions();
	}

	void GameScene::render() {
		auto &positions = _registry.get_components<component::position>();
		auto &drawables = _registry.get_components<component::drawable>();

		_raylib.beginDrawing();
		_raylib.clearBackground(BLACK);
		const std::size_t n = std::min(positions.size(), drawables.size());
		for (std::size_t i = 0; i < n; ++i) {
			if (positions[i] && drawables[i]) {
				Rectangle rec = {positions[i]->x, positions[i]->y, drawables[i]->width, drawables[i]->height};
				Color color = {
					static_cast<unsigned char>(std::clamp(drawables[i]->r, 0.f, 1.f) * 255.f),
					static_cast<unsigned char>(std::clamp(drawables[i]->g, 0.f, 1.f) * 255.f),
					static_cast<unsigned char>(std::clamp(drawables[i]->b, 0.f, 1.f) * 255.f),
					static_cast<unsigned char>(std::clamp(drawables[i]->a, 0.f, 1.f) * 255.f)
				};
				_raylib.drawRectangleRec(rec, color);
			}
		}
		_raylib.endDrawing();
	}

	void GameScene::handleEvents() {
		update();

		switch (_raylib.getKeyPressed()) {
			default:
				break;
		}

		// Player movements
		float input_x = 0.f;
		float input_y = 0.f;

		if (_raylib.isKeyDown(KEY_W) || _raylib.isKeyDown(KEY_UP)) input_y = -1.f;
		if (_raylib.isKeyDown(KEY_S) || _raylib.isKeyDown(KEY_DOWN)) input_y = 1.f;
		if (_raylib.isKeyDown(KEY_A) || _raylib.isKeyDown(KEY_LEFT)) input_x = -1.f;
		if (_raylib.isKeyDown(KEY_D) || _raylib.isKeyDown(KEY_RIGHT)) input_x = 1.f;


		handle_input(input_x, input_y);
	}

	void GameScene::onClose() {
		_game_running = false;
	}

	void GameScene::handle_input(float input_x, float input_y) {
		auto &velocities = _registry.get_components<component::velocity>();
		auto &controllables = _registry.get_components<component::controllable>();

		if (_player.value() < velocities.size() && velocities[_player.value()] &&
			_player.value() < controllables.size() && controllables[_player.value()]) {
			velocities[_player.value()]->vx = input_x * controllables[_player.value()]->speed;
			velocities[_player.value()]->vy = input_y * controllables[_player.value()]->speed;
		}
	}

	void GameScene::setup_movement_system() {
		_registry.add_system<component::position, component::velocity>(
			[](ecs::registry &reg,
				ecs::sparse_array<component::position> &pos,
				ecs::sparse_array<component::velocity> &vel) {
				float dt = 0.016f;

				for (std::size_t i = 0; i < pos.size() && i < vel.size(); ++i) {
					if (pos[i] && vel[i]) {
						pos[i]->x += vel[i]->vx * dt;
						pos[i]->y += vel[i]->vy * dt;
					}
				}
			});
	}

	void GameScene::setup_render_system() {
		_registry.add_system<component::position, component::drawable>(
			[this](ecs::registry &reg,
					ecs::sparse_array<component::position> &pos,
					ecs::sparse_array<component::drawable> &drw) {
			});
	}

	void GameScene::setup_collision_system() {
	}

	void GameScene::setup_health_system() {
		_registry.add_system<component::health, component::type>(
			[](ecs::registry &reg,
				ecs::sparse_array<component::health> &health,
				ecs::sparse_array<component::type> &type) {
				for (std::size_t i = 0; i < health.size() && i < type.size(); ++i) {
					if (health[i] && type[i] && health[i]->current <= 0) {
						ecs::entity_t entity = reg.entity_from_index(i);
						reg.kill_entity(entity);
					}
				}
			});
	}

	void GameScene::create_player() {
		_player = game::entities::create_player(_registry, 100.f, 300.f);
	}

	/* ----- METTRE CA DANS UN ENEMY.CPP???? (COMME PLAYER) -------- */
	void GameScene::create_enemies() {
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<float> y_dist(50.f, 550.f);

		auto enemy = _registry.spawn_entity();

		_registry.emplace_component<component::position>(enemy, static_cast<float>(_width - 50), y_dist(gen));

		_registry.emplace_component<component::velocity>(enemy, -100.f, 0.f);

		_registry.emplace_component<component::type>(enemy, component::entity_type::ENEMY);

		_registry.emplace_component<component::health>(enemy, 50, 50);

		_registry.emplace_component<component::damage>(enemy, 20);

		_registry.emplace_component<component::collision_box>(enemy, 40.f, 40.f);

		component::drawable drawable;
		drawable.width = 40.f;
		drawable.height = 40.f;
		drawable.r = 1.f;
		drawable.g = 0.f;
		drawable.b = 0.f;
		drawable.a = 1.f;
		_registry.add_component<component::drawable>(enemy, std::move(drawable));

		_enemies.push_back(enemy);
	}

	/* ----- METTRE CA DANS UN OBSTACLES.CPP???? (COMME PLAYER) -------- */

	void GameScene::create_obstacles() {
		for (int i = 0; i < 3; ++i) {
			auto obstacle = _registry.spawn_entity();

			_registry.emplace_component<component::position>(obstacle, 200.f + i * 200.f, 400.f);

			_registry.emplace_component<component::type>(obstacle, component::entity_type::OBSTACLE);

			_registry.emplace_component<component::collision_box>(obstacle, 60.f, 60.f);

			component::drawable drawable;
			drawable.width = 60.f;
			drawable.height = 60.f;
			drawable.r = 0.4f;
			drawable.g = 0.4f;
			drawable.b = 0.4f;
			drawable.a = 1.f;
			_registry.add_component<component::drawable>(obstacle, std::move(drawable));

			_obstacles.push_back(obstacle);
		}
	}

	void GameScene::check_collisions() {
		// Implémentation basique des collisions
	}
} // namespace game::scene
