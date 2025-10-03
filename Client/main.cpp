/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** main
*/

#include "../Engine/Game.hpp"

int main(int argc, char *argv[]) {
	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <server_ip> <client_name>" << std::endl;
		return 1;
	}
	Game game(argv[1], argv[2]);
	game.run();
	return 0;
}