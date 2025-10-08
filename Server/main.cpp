/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** main
*/

#include "server.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <server_port> " << std::endl;
		return 1;
	}
    GameServer server(static_cast<uint16_t>(std::stoi(argv[1])));
    server.run();
    return 0;
}