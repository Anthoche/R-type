/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Game
*/

#include "Game.hpp"

Game::Game(const std::string &serverIp, const std::string &serverPort, const std::string &clientName) 
    : _sceneHandler(*this),
      _client(*this, serverIp, static_cast<uint16_t>(std::stoi(serverPort)), clientName) {
    _status = GameStatus::WAITING_PLAYERS;
    _selectedSkinPath = ASSETS_PATH "/sprites/player/r-typesheet42.png";
    _selectedWeaponPath.clear();
    _selectedWeaponId = "basic_shot";
}

void Game::run() {
    _networkThread = std::thread([this]() {
		_client.run();
	});
	_sceneHandler.openMenu();

	if (_client.roomId >= 0)
		_client.sendClientLeaveRoom();

	_client.disconnect();
	if (_networkThread.joinable())
		_networkThread.join();
}

bool GameClient::isConnected() const { 
	return clientId > 0;
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
