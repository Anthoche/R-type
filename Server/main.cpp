/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** main
*/

#include "Include/server.hpp"
#include "Logger.hpp"

bool is_number(std::string const &str) {
	std::string::const_iterator it = str.begin();

	while (it != str.end()) {
		if (!std::isdigit(*it))
			return false;
		++it;
	}
	return !str.empty();
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		LOG_ERROR("You must enter a port for the server to listen on.");
		std::cerr << "Usage: " << argv[0] << " <server_port> " << std::endl;
		return 1;
	}
	std::string const port = std::string(argv[1]);

	LOG_INFO("Starting server...");
	if (port == "" || !is_number(port)) {
		LOG_ERROR("The port number entered is invalid! Port must not be empty and contains only numbers!");
		std::cerr << "Usage: " << argv[0] << " <server_port> " << std::endl;
		return 1;
	}
	GameServer server(static_cast<uint16_t>(std::stoi(port)));
	server.run();
	return 0;
}
