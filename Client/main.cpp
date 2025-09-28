/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** main
*/

#include "../Game/Game.hpp"

int main(int argc, char *argv[]) {
	std::string serverIp;

	if (argc < 3) {
		std::cerr << "Usage: " << argv[0] << " <server_ip> <client_name>" << std::endl;
		return 1;
	}
	serverIp = argv[1];
	if (serverIp == "localhost")
		serverIp = "127.0.0.1";
	Game game(serverIp, argv[2]);
	game.run();
	return 0;
}
