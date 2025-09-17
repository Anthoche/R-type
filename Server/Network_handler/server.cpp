/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#include "Include/server.hpp"
#include <iostream>

GameServer::GameServer(uint16_t port) : socket(port) {}

void GameServer::run() {
    std::cout << "Serveur démarré. En attente de 4 clients..." << std::endl;

    while (!gameStarted) {
        auto [data, clientAddr] = socket.receive();
        if (data.size() < sizeof(MessageType)) {
            continue;
        }
        MessageType type = *reinterpret_cast<MessageType*>(data.data());
        if (type == MessageType::ClientHello) {
            handleClientHello(data, clientAddr);
        }
    }
    std::cout << "Tous les clients sont connectés. Le jeu commence !" << std::endl;
}

void GameServer::handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr) {
    if (gameStarted) {
        return;
    }
    if (data.size() < sizeof(ClientHelloMessage)) {
        return;
    }
    const ClientHelloMessage* msg = reinterpret_cast<const ClientHelloMessage*>(data.data());
    uint32_t clientId = nextClientId++;
    socket.addClient(clientAddr, clientId);
    ServerAssignIdMessage assignMsg;
    assignMsg.type = MessageType::ServerAssignId;
    assignMsg.clientId = htonl(clientId);
    socket.sendTo(&assignMsg, sizeof(assignMsg), clientAddr);
    std::cout << "Client connecté : " << msg->clientName
              << " (ID: " << clientId << ")"
              << " [" << socket.getClientCount() << "/4]" << std::endl;
    if (socket.getClientCount() == 4) {
        gameStarted = true;
        broadcastGameStart();
    }
}

void GameServer::broadcastGameStart() {
    GameStartMessage msg;
    msg.type = MessageType::GameStart;
    msg.clientCount = htonl(4);
    socket.broadcast(&msg, sizeof(msg));
    std::cout << "[DEBUG] Message GameStart envoyé à tous les clients." << std::endl;
}
