/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** UDP_socket
*/


#include "Include/UDP_socket.hpp"


UDP_socket::UDP_socket(uint16_t port) {
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketFd < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    if (bind(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }
}

UDP_socket::~UDP_socket() {
    close(socketFd);
}

std::pair<std::vector<uint8_t>, sockaddr_in> UDP_socket::receive() {
    std::vector<uint8_t> buffer(1024);
    sockaddr_in clientAddr = {};
    socklen_t clientAddrLen = sizeof(clientAddr);
    ssize_t bytesReceived = recvfrom(
        socketFd, buffer.data(), buffer.size(), 0,
        (struct sockaddr*)&clientAddr, &clientAddrLen
    );

    if (bytesReceived < 0) {
        throw std::runtime_error("Failed to receive data");
    }
    buffer.resize(bytesReceived);

    return {buffer, clientAddr};
}

void UDP_socket::sendTo(const void* data, size_t size, const sockaddr_in& clientAddr) {
    sendto(
        socketFd, data, size, 0,
        (struct sockaddr*)&clientAddr, sizeof(clientAddr)
    );
}

void UDP_socket::broadcast(const void* data, size_t size) {
    for (const auto& [addrStr, clientId] : clients) {
        sockaddr_in addr = {};
        sscanf(addrStr.c_str(), "%hu.%hu.%hu.%hu:%hu",
            &addr.sin_addr.s_addr, &addr.sin_addr.s_addr + 1,
            &addr.sin_addr.s_addr + 2, &addr.sin_addr.s_addr + 3,
            &addr.sin_port);
        addr.sin_family = AF_INET;
        sendTo(data, size, addr);
    }
}

void UDP_socket::addClient(const sockaddr_in& clientAddr, uint32_t clientId) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
    std::string addrStr = std::string(ipStr) + ":" + std::to_string(ntohs(clientAddr.sin_port));
    clients[addrStr] = clientId;
}

size_t UDP_socket::getClientCount() const {
    return clients.size();
}
