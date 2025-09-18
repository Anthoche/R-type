/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/


#pragma once

#include "UDP_socket.hpp"
#include "../../../Shared/protocol.hpp"
#include <vector>
#include <unordered_map>
#include <sys/socket.h>
#include <netinet/in.h>

class Connexion {
    public:
        Connexion(uint16_t port);
        ~Connexion() = default;

        bool tryReceive(std::vector<uint8_t>& data, sockaddr_in& from);
        void sendTo(const void* msg, size_t size, const sockaddr_in& to);
        void broadcast(const void* msg, size_t size);
        void addClient(const sockaddr_in& addr, uint32_t id);
        size_t getClientCount() const;
        const std::unordered_map<std::string, uint32_t>& getClients() const;

    private:
        UDP_socket socket;
        std::unordered_map<std::string, uint32_t> clients;
};
