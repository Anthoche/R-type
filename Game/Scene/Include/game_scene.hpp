#pragma once

#include "registry.hpp"
#include "entity.hpp"
#include "../../Entities/Include/components.hpp"

namespace game::scene {

class GameScene {
public:
	GameScene(int width = 800, int height = 600);
	~GameScene() = default;
	
	// Initialisation de la scène
	void initialize();
	
	// Mise à jour de la logique de jeu
	void update(float delta_time);
	
	// Rendu de la scène
	void render();
	
	// Gestion des événements
	void handle_input(float input_x, float input_y);
	
	// Nettoyage
	void cleanup();

private:
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
	
	// Variables membres
	ecs::registry _registry;
	int _window_width;
	int _window_height;
	
	// Entités principales
	ecs::entity_t _player;
	std::vector<ecs::entity_t> _enemies;
	std::vector<ecs::entity_t> _obstacles;
	
	// État du jeu
	bool _game_running{true};
	float _enemy_spawn_timer{0.f};
	const float _enemy_spawn_interval{2.0f};
};

} // namespace game::scene
