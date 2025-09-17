#include "Scene/Include/game_scene.hpp"
#include <iostream>
#include <chrono>

int main() {
	// Création de la scène de jeu
	game::scene::GameScene game_scene(800, 600);
	
	auto last_time = std::chrono::high_resolution_clock::now();
	while (game_scene.window_is_open()) {
		auto current_time = std::chrono::high_resolution_clock::now();
		float delta_time = std::chrono::duration<float>(current_time - last_time).count();
		last_time = current_time;
		
		game_scene.poll_events();
		// TODO: remplacer par un vrai input; ici on ne force aucun déplacement
		game_scene.handle_input(0.f, 0.f);
		game_scene.update(delta_time);
		game_scene.render();
	}
	
	// Nettoyage
	game_scene.cleanup();
	return 0;
}
