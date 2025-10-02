/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client_network - Network communication
*/

#include "../client.hpp"

void GameClient::sendHello() {
    ClientHelloMessage msg;
    msg.type = MessageType::ClientHello;
    msg.clientId = htonl(0);
    strncpy(msg.clientName, clientName.c_str(), sizeof(msg.clientName) - 1);
    msg.clientName[sizeof(msg.clientName) - 1] = '\0';
    ssize_t sentBytes = sendto(
        socketFd, &msg, sizeof(msg), 0,
        (struct sockaddr *)&serverAddr, sizeof(serverAddr)
    );
    if (sentBytes < 0) {
        std::cerr << "[ERREUR] Échec de l'envoi de ClientHello" << std::endl;
    }
}

void GameClient::sendInput(float inputX, float inputY) {
    ClientInputMessage m{};
    m.type = MessageType::ClientInput;
    m.clientId = htonl(clientId);
    uint32_t xbits, ybits;
    std::memcpy(&xbits, &inputX, sizeof(float));
    std::memcpy(&ybits, &inputY, sizeof(float));
    m.inputXBits = htonl(xbits);
    m.inputYBits = htonl(ybits);
    sendto(
        socketFd, &m, sizeof(m), 0,
        (struct sockaddr *)&serverAddr, sizeof(serverAddr)
    );
}

void GameClient::recvLoop() {
    while (running) {
        std::vector<uint8_t> buffer(1024);
        sockaddr_in fromAddr = {};
        socklen_t fromAddrLen = sizeof(fromAddr);
        ssize_t bytesReceived = recvfrom(
            socketFd, buffer.data(), buffer.size(), MSG_DONTWAIT,
            (struct sockaddr *)&fromAddr, &fromAddrLen
        );
        if (bytesReceived <= 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            continue;
        }
        if (bytesReceived < (ssize_t)sizeof(MessageType)) {
            continue;
        }
        MessageType type = *reinterpret_cast<MessageType *>(buffer.data());
        handleMessage(type, buffer);
    }
}