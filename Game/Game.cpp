/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Game
*/

#include "Game.hpp"

Game::Game(const std::string &serverIp, const std::string &clientName) : _client(serverIp, 4141, clientName), _sceneHandler(*this) {}

void Game::run() {
	_networkThread = std::thread([this]() {
		_client.run();
	});

	_networkThread.join();

	std::cout << "Ouverture de la fenêtre de jeu..." << std::endl;
	_sceneHandler.openMenu();
}
