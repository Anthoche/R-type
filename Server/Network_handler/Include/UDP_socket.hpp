/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** udp_socket
*/

#pragma once

#include <asio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <cstdint>
#include <iostream>

class UDP_socket {
public:
    explicit UDP_socket(uint16_t port);
    ~UDP_socket();

    // --- réception ---
    std::pair<std::vector<uint8_t>, asio::ip::udp::endpoint> receive(); // blocant
    bool try_receive(std::vector<uint8_t>& outData, asio::ip::udp::endpoint& outEndpoint); // non bloquant

    // --- envoi ---
    void sendTo(const void* data, size_t size, const asio::ip::udp::endpoint& endpoint);
    void broadcast(const void* data, size_t size);

    // --- gestion clients ---
    void addClient(const asio::ip::udp::endpoint& endpoint, uint32_t clientId);
    void disconnectClient(uint32_t clientId); // suppression client
    size_t getClientCount() const;

    const std::unordered_map<std::string, uint32_t>& getClients() const { return clients; }

private:
    asio::io_context ioContext;
    asio::ip::udp::socket socket;
    std::unordered_map<std::string, uint32_t> clients;
    mutable std::mutex clientsMutex;
};
