/*
** EPITECH PROJECT, 2025
** G-CCP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#pragma once

#include "UDP_socket.hpp"
#include "TCP_socketServer.hpp"
#include "../../../Shared/protocol.hpp"
#include <vector>
#include <unordered_map>
#include <asio.hpp>
#include <functional>
#include <iostream>
#include <memory>

/**
 * @brief High-level connection manager for UDP network operations.
 *
 * Wraps around UDP_socket, providing simplified API for receiving,
 * sending, broadcasting messages, and client management.
 */
class Connexion {
    public:
        Connexion(asio::io_context& io, uint16_t port);
        ~Connexion() = default;

        void asyncReceive(std::function<void(const asio::error_code&, std::vector<uint8_t>, asio::ip::udp::endpoint)> handler);
        void sendTo(const void* msg, size_t size, const asio::ip::udp::endpoint& to);
        void broadcast(const void* msg, size_t size);

        void addClient(const asio::ip::udp::endpoint& endpoint, uint32_t id);
        void disconnectClient(uint32_t id);

        size_t getClientCount() const;
        const std::unordered_map<std::string, uint32_t>& getClients() const;
        const std::unordered_map<std::string, asio::ip::udp::endpoint>& getEndpoints() const;

        bool acceptTcpClient(uint32_t id, uint16_t port);
        void sendJsonToClient(uint32_t id, const nlohmann::json& j);
        void broadcastJson(const nlohmann::json& j);

    private:
        UDP_socket socket;
        std::unordered_map<std::string, uint32_t> clients;
        std::unordered_map<std::string, asio::ip::udp::endpoint> endpoints;
        std::unordered_map<uint32_t, std::shared_ptr<TCP_socketServer>> tcpClients;
};
