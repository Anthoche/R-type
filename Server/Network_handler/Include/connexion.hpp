/*
** EPITECH PROJECT, 2025
** G-CCP-500-PAR-5-1-rtype-1
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

/**
 * @brief High-level connection manager for UDP network operations.
 *
 * Acts as a facade for UDP_socket, providing a simplified API
 * for receiving, sending, and broadcasting messages, as well as client management.
 */
class Connexion {
    public:
        /**
         * @brief Constructs a Connexion instance bound to the specified port.
         * @param port The UDP port to listen on.
         */
        Connexion(uint16_t port);

        ~Connexion() = default;

        /**
         * @brief Attempts to receive a UDP message (non-blocking).
         * @param data Output parameter for the received data.
         * @param from Output parameter for the sender's address.
         * @return true if a message was received, false otherwise.
         */
        bool tryReceive(std::vector<uint8_t>& data, sockaddr_in& from);

        /**
         * @brief Sends a UDP message to a specific client.
         * @param msg Pointer to the message data.
         * @param size Size of the message in bytes.
         * @param to Destination address.
         */
        void sendTo(const void* msg, size_t size, const sockaddr_in& to);

        /**
         * @brief Broadcasts a UDP message to all connected clients.
         * @param msg Pointer to the message data.
         * @param size Size of the message in bytes.
         */
        void broadcast(const void* msg, size_t size);

        /**
         * @brief Registers a new client with the connection manager.
         * @param addr Address of the client to add.
         * @param id Unique ID to assign to the client.
         */
        void addClient(const sockaddr_in& addr, uint32_t id);

        /**
         * @brief Gets the number of connected clients.
         * @return Current client count.
         */
        size_t getClientCount() const;

        /**
         * @brief Gets the map of connected clients.
         * @return A const reference to the clients map (IP string -> client ID).
         */
        const std::unordered_map<std::string, uint32_t>& getClients() const;

    private:
        UDP_socket socket; ///< Underlying UDP socket for network operations.
        std::unordered_map<std::string, uint32_t> clients; ///< Maps client IP strings to their assigned IDs.
};
