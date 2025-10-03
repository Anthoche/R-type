/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/
#include "Include/connexion.hpp"
#include <arpa/inet.h>
#include <iostream>

// Codes ANSI pour les couleurs
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

Connexion::Connexion(uint16_t port) : socket(port) {}

bool Connexion::tryReceive(std::vector<uint8_t>& data, sockaddr_in& from) {
    bool success = socket.try_receive(data, from);
    if (success) {
        char ipStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &from.sin_addr, ipStr, INET_ADDRSTRLEN);
        std::cout << GREEN << "[RECV] Received data from client: " << ipStr
                  << " (" << ntohs(from.sin_port) << ")" << RESET << std::endl;
    }
    return success;
}

void Connexion::sendTo(const void* msg, size_t size, const sockaddr_in& to) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &to.sin_addr, ipStr, INET_ADDRSTRLEN);
    std::cout << GREEN << "[SEND] Sending data to client: " << ipStr
              << " (" << ntohs(to.sin_port) << "), size: " << size << RESET << std::endl;
    socket.sendTo(msg, size, to);
}

void Connexion::broadcast(const void* msg, size_t size) {
    std::cout << GREEN << "[SEND] Broadcasting data to all clients, size: " << size << RESET << std::endl;
    socket.broadcast(msg, size);
}

void Connexion::addClient(const sockaddr_in& addr, uint32_t id) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ipStr, INET_ADDRSTRLEN);
    clients[ipStr] = id;
    socket.addClient(addr, id);
    std::cout << GREEN << "[INFO] Added client: " << ipStr
              << " with ID: " << id << RESET << std::endl;
}

size_t Connexion::getClientCount() const {
    return clients.size();
}

const std::unordered_map<std::string, uint32_t>& Connexion::getClients() const {
    return clients;
}

