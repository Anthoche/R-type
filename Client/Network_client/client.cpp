/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#include "Include/client.hpp"


GameClient::GameClient(const std::string& serverIp, uint16_t serverPort, const std::string& name)
    : clientName(name) {
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
    sendHello();
}

GameClient::~GameClient() {
    close(socketFd);
}

void GameClient::run() {
    std::cout << "Client démarré. En attente du début du jeu..." << std::endl;

    while (true) {
        std::vector<uint8_t> buffer(1024);
        sockaddr_in fromAddr = {};
        socklen_t fromAddrLen = sizeof(fromAddr);
        ssize_t bytesReceived = recvfrom(
            socketFd, buffer.data(), buffer.size(), 0,
            (struct sockaddr*)&fromAddr, &fromAddrLen
        );

        if (bytesReceived < sizeof(MessageType)) {
            continue;
        }
        MessageType type = *reinterpret_cast<MessageType*>(buffer.data());
        if (type == MessageType::GameStart) {
            if (bytesReceived >= sizeof(GameStartMessage)) {
                const GameStartMessage* msg = reinterpret_cast<const GameStartMessage*>(buffer.data());
                std::cout << "Le jeu commence ! Nombre de joueurs : " << msg->clientCount << std::endl;
                break;
            }
        }
    }
}

void GameClient::sendHello() {
    ClientHelloMessage msg;
    msg.clientId = 0;
    strncpy(msg.clientName, clientName.c_str(), sizeof(msg.clientName) - 1);
    msg.clientName[sizeof(msg.clientName) - 1] = '\0';
    sendto(
        socketFd, &msg, sizeof(msg), 0,
        (struct sockaddr*)&serverAddr, sizeof(serverAddr)
    );
}
