/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Game
*/

#include "Game.hpp"

Game::Game(const std::string &serverIp, const std::string &clientName) : _client(serverIp, 4141, clientName), _sceneHandler(*this) {
}

void Game::run() {
	// Lancer le client réseau dans un thread séparé
	_networkThread = std::thread([this]() {
		_client.run();
	});

	// Attendre que le thread réseau se termine (quand GameStart est reçu)
	_networkThread.join();

	// Maintenant que le jeu peut commencer, ouvrir la fenêtre
	std::cout << "Ouverture de la fenêtre de jeu..." << std::endl;
	_sceneHandler.openMenu();
}
