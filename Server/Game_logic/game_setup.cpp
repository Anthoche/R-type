/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** game_setup
*/
#include "Include/game_setup.hpp"
#include <iostream>
#include <thread>
#include <chrono>


GameSetup::GameSetup(Connexion& connexion) : connexion(connexion) {}

bool GameSetup::waitForPlayers() {
    std::cout << "Serveur démarré. En attente de 4 clients..." << std::endl;
    while (!gameStarted) {
        std::vector<uint8_t> data;
        sockaddr_in clientAddr = {};
        if (!connexion.tryReceive(data, clientAddr)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }
        if (data.size() < sizeof(MessageType)) {
            continue;
        }
        MessageType type = *reinterpret_cast<MessageType*>(data.data());
        if (type == MessageType::ClientHello) {
            handleClientHello(data, clientAddr);
        }
    }
    return true;
}

void GameSetup::handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr) {
    if (gameStarted) {
        return;
    }
    if (data.size() < sizeof(ClientHelloMessage)) {
        return;
    }
    const ClientHelloMessage* msg = reinterpret_cast<const ClientHelloMessage*>(data.data());
    uint32_t clientId = nextClientId++;
    connexion.addClient(clientAddr, clientId);
    ServerAssignIdMessage assignMsg;
    assignMsg.type = MessageType::ServerAssignId;
    assignMsg.clientId = htonl(clientId);
    connexion.sendTo(&assignMsg, sizeof(assignMsg), clientAddr);
    std::cout << "Client connecté : " << msg->clientName
              << " (ID: " << clientId << ")"
              << " [" << connexion.getClientCount() << "/4]" << std::endl;
    if (connexion.getClientCount() == 4) {
        gameStarted = true;
    }
}

void GameSetup::broadcastGameStart() {
    GameStartMessage msg;
    msg.type = MessageType::GameStart;
    msg.clientCount = htonl(4);
    connexion.broadcast(&msg, sizeof(msg));
    std::cout << "[DEBUG] Message GameStart envoyé à tous les clients." << std::endl;
}
