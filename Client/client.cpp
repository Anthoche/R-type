/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#include "client.hpp"
#include "../Engine/Game.hpp"

GameClient::GameClient(Game &game, const std::string &serverIp, uint16_t serverPort, const std::string &name)
    : clientName(name), _game(game) {
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid server IP");
    }
}

GameClient::~GameClient() {
    running = false;
    if (rxThread.joinable())
        rxThread.join();
    close(socketFd);
}

void GameClient::run() {
    std::cout << "Client démarré. En attente du début du jeu..." << std::endl;
    running = true;
    rxThread = std::thread(&GameClient::recvLoop, this);
    while (_game.getGameStatus() != GameStatus::RUNNING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "Le jeu commence, ouverture de la fenêtre client..." << std::endl;
}
