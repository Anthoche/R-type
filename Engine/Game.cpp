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
}

void Game::run() {
    _networkThread = std::thread([this]() {
        _client.run();
    });
    auto startTime = std::chrono::steady_clock::now();
    const int MAX_WAIT = 6;
    
    while (!_client.isConnected() && !_client.hasConnectionFailed()) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        if (elapsed >= MAX_WAIT) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    if (_client.hasConnectionFailed()) {
        std::cerr << "\n[GAME] Impossible de se connecter au serveur." << std::endl;
        std::cerr << "[GAME] Le programme va se terminer.\n" << std::endl;
        _networkThread.join();
        return;
    }
    std::cout << "[GAME] Connexion réussie, ouverture de l'interface..." << std::endl;
    _sceneHandler.openMenu();  
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