#include "Scene/Include/game_scene.hpp"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
	std::cout << "=== R-Type ECS Game (Version sans SFML) ===" << std::endl;
	
	// Création de la scène de jeu
	game::scene::GameScene game_scene(800, 600);
	
	// Boucle principale simplifiée
	auto last_time = std::chrono::high_resolution_clock::now();
	bool running = true;
	
	while (running) {
		auto current_time = std::chrono::high_resolution_clock::now();
		float delta_time = std::chrono::duration<float>(current_time - last_time).count();
		last_time = current_time;
		
		// Simulation d'input simple (mouvement vers la droite)
		static float input_x = 1.0f;
		static float input_y = 0.0f;
		
		// Gestion de l'input
		game_scene.handle_input(input_x, input_y);
		
		// Mise à jour de la logique de jeu
		game_scene.update(delta_time);
		
		// Rendu
		game_scene.render();
		
		// Petite pause pour éviter de surcharger le CPU
		std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
		
		// Arrêt après quelques secondes pour la démo
		static float demo_time = 0.0f;
		demo_time += delta_time;
		if (demo_time > 5.0f) {
			running = false;
		}
	}
	
	// Nettoyage
	game_scene.cleanup();
	
	std::cout << "Fin de la démo" << std::endl;
	return 0;
}
