/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** UDP_socket
*/


#include "Include/UDP_socket.hpp"
#include <fcntl.h>
#include <errno.h>


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
    // Set non-blocking mode
    int flags = fcntl(socketFd, F_GETFL, 0);
    if (flags != -1) {
        fcntl(socketFd, F_SETFL, flags | O_NONBLOCK);
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

bool UDP_socket::try_receive(std::vector<uint8_t>& outData, sockaddr_in& outAddr) {
    outData.resize(1024);
    socklen_t addrLen = sizeof(outAddr);
    ssize_t bytesReceived = recvfrom(
        socketFd, outData.data(), outData.size(), 0,
        (struct sockaddr*)&outAddr, &addrLen
    );
    if (bytesReceived < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return false;
        }
        return false;
    }
    outData.resize(bytesReceived);
    return true;
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
        addr.sin_family = AF_INET;
        size_t colonPos = addrStr.find(':');
        std::string ipStr = addrStr.substr(0, colonPos);
        uint16_t port = static_cast<uint16_t>(std::stoi(addrStr.substr(colonPos + 1)));

        inet_pton(AF_INET, ipStr.c_str(), &addr.sin_addr);
        addr.sin_port = htons(port);
        ssize_t sentBytes = sendto(
            socketFd, data, size, 0,
            (struct sockaddr*)&addr, sizeof(addr)
        );
        if (sentBytes < 0) {
            std::cerr << "[ERREUR] Échec de l'envoi à " << addrStr << std::endl;
        }
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
