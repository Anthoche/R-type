/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Game
*/

#include "Game.hpp"


Game::Game(const std::string &serverIp, const std::string &clientName) : _sceneHandler(*this),
																		_client(*this, serverIp, 4141, clientName) {
	_status = GameStatus::WAITING_PLAYERS;
}

void Game::run() {
	_networkThread = std::thread([this]() {
		_client.run();
	});
	_sceneHandler.openMenu();
	_networkThread.join();
}

SceneHandler &Game::getSceneHandler() {
	return _sceneHandler;
}

GameClient &Game::getGameClient() {
	return _client;
}

GameStatus Game::getGameStatus() const {
	return _status;
}

void Game::setGameStatus(const GameStatus &status) {
	_status = status;
}
