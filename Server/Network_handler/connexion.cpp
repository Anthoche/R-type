/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#include "Include/connexion.hpp"
#include <arpa/inet.h>

Connexion::Connexion(uint16_t port) : socket(port) {}

bool Connexion::tryReceive(std::vector<uint8_t>& data, sockaddr_in& from) {
    return socket.try_receive(data, from);
}

void Connexion::sendTo(const void* msg, size_t size, const sockaddr_in& to) {
    socket.sendTo(msg, size, to);
}

void Connexion::broadcast(const void* msg, size_t size) {
    socket.broadcast(msg, size);
}

void Connexion::addClient(const sockaddr_in& addr, uint32_t id) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ipStr, INET_ADDRSTRLEN);
    clients[ipStr] = id;
    socket.addClient(addr, id);
}

size_t Connexion::getClientCount() const {
    return clients.size();
}

const std::unordered_map<std::string, uint32_t>& Connexion::getClients() const {
    return clients;
}
