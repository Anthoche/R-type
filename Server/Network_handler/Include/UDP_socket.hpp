/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** UDP_socket
*/


#pragma once

#include <unordered_map>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>


class UDP_socket {
    int socketFd;
    sockaddr_in serverAddr;
    std::unordered_map<std::string, uint32_t> clients;

    public:
        UDP_socket(uint16_t port);
        ~UDP_socket();

        std::pair<std::vector<uint8_t>, sockaddr_in> receive();
        // Non-blocking receive; returns false if no data available
        bool try_receive(std::vector<uint8_t>& outData, sockaddr_in& outAddr);
        void sendTo(const void* data, size_t size, const sockaddr_in& clientAddr);
        void broadcast(const void* data, size_t size);
        void addClient(const sockaddr_in& clientAddr, uint32_t clientId);
        size_t getClientCount() const;
        const std::unordered_map<std::string, uint32_t>& getClients() const { return clients; }
};
