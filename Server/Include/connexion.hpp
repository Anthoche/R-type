/*
** EPITECH PROJECT, 2025
** G-CCP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#pragma once

#include "../../Shared/Sockets/Include/UDP_socket.hpp"
#include "../../Shared/Sockets/Include/TCP_socket.hpp"
#include "../../Shared/protocol.hpp"
#include <vector>
#include <unordered_map>
#include <asio.hpp>
#include <functional>
#include <iostream>
#include <memory>

/**
 * @class Connexion
 * @brief High-level UDP connection manager with optional TCP support.
 *
 * This class wraps around a UDP_socket to provide a simplified interface for:
 * - Receiving UDP messages asynchronously.
 * - Sending UDP messages to specific clients or broadcasting.
 * - Managing connected clients and their endpoints.
 * - Handling TCP connections for reliable communication.
 */
class Connexion {
    public:
        /**
         * @brief Constructs a Connexion instance and binds the UDP socket to a port.
         * @param io ASIO io_context used for asynchronous operations.
         * @param port UDP port to bind to.
         */
        Connexion(asio::io_context& io, uint16_t port);

        /**
         * @brief Destroys the Connexion instance.
         */
        ~Connexion() = default;

        /**
         * @brief Receives a UDP message asynchronously.
         *
         * Calls the provided handler if a message is available.
         *
         * @param handler Callback invoked with the error code, message data, and sender endpoint.
         */
        void asyncReceive(std::function<void(const asio::error_code&, std::vector<uint8_t>, asio::ip::udp::endpoint)> handler);

        /**
         * @brief Sends a UDP message to a specific client.
         * @param msg Pointer to the data to send.
         * @param size Size of the data in bytes.
         * @param to Destination UDP endpoint.
         */
        void sendTo(const void* msg, size_t size, const asio::ip::udp::endpoint& to);

        /**
         * @brief Broadcasts a UDP message to all connected clients.
         * @param msg Pointer to the data to broadcast.
         * @param size Size of the data in bytes.
         */
        void broadcast(const void* msg, size_t size);

        /**
         * @brief Registers a new client with the connection manager.
         * @param endpoint UDP endpoint of the client.
         * @param id Unique client ID.
         */
        void addClient(const asio::ip::udp::endpoint& endpoint, uint32_t id);

        /**
         * @brief Disconnects a client and removes it from the manager.
         * @param id ID of the client to disconnect.
         */
        void disconnectClient(uint32_t id);

        /**
         * @brief Returns the number of currently connected clients.
         * @return Number of clients.
         */
        size_t getClientCount() const;

        /**
         * @brief Returns a map of client addresses to their IDs.
         * @return const reference to the clients map.
         */
        const std::unordered_map<std::string, uint32_t>& getClients() const;

        /**
         * @brief Returns a map of client addresses to their UDP endpoints.
         * @return const reference to the endpoints map.
         */
        const std::unordered_map<std::string, asio::ip::udp::endpoint>& getEndpoints() const;

        /**
         * @brief Accepts a TCP connection from a client on the given port.
         * @param id Client ID.
         * @param port TCP port to listen on.
         * @return true if the client was accepted successfully, false otherwise.
         */
        bool acceptTcpClient(uint32_t id, uint16_t port);

        /**
         * @brief Sends a JSON object to a specific TCP client.
         * @param id Client ID.
         * @param j JSON object to send.
         */
        void sendJsonToClient(uint32_t id, const nlohmann::json& j);

        /**
         * @brief Broadcasts a JSON object to all connected TCP clients.
         * @param j JSON object to broadcast.
         */
        void broadcastJson(const nlohmann::json& j);

    private:
        UDP_socket socket; ///< UDP socket instance for message transmission.
        std::unordered_map<std::string, uint32_t> clients; ///< Maps client addresses to their IDs.
        std::unordered_map<std::string, asio::ip::udp::endpoint> endpoints; ///< Maps client addresses to UDP endpoints.
        std::unordered_map<uint32_t, std::shared_ptr<TCP_socket>> tcpClients; ///< TCP connections for reliable messages.
};
