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

void GameSetup::startWaiting() {
    std::cout << "Serveur démarré. En attente de 4 clients..." << std::endl;
    doAsyncReceive();
}

void GameSetup::broadcastGameStart() {
    GameStartMessage msg;
    msg.type = MessageType::GameStart;
    msg.clientCount = htonl(4);
    connexion.broadcast(&msg, sizeof(msg));
    std::cout << "[DEBUG] Message GameStart envoyé à tous les clients." << std::endl;
}

bool GameSetup::isGameStarted() const {  
    return gameStarted;
}

void GameSetup::doAsyncReceive() {
    connexion.asyncReceive(
        [this](const asio::error_code& ec, std::vector<uint8_t> data, asio::ip::udp::endpoint from) {
            if (ec) {
                std::cerr << "[ERREUR] Réception UDP: " << ec.message() << std::endl;
                return;
            }
            if (data.size() >= sizeof(MessageType)) {
                MessageType type = *reinterpret_cast<MessageType*>(data.data());
                if (type == MessageType::ClientHello) {
                    handleClientHello(data, from);
                }
            }
            if (!gameStarted) {
                doAsyncReceive();
            }
        }
    );
}

void GameSetup::handleClientHello(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& clientAddr) {
    if (gameStarted)
        return;
    if (data.size() < sizeof(ClientHelloMessage))
        return;
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
        broadcastGameStart();
    }
}