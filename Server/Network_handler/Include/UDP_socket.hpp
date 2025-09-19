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

/**
 * @brief UDP socket wrapper for non-blocking network communication.
 *
 * Manages a UDP socket, client connections, and message sending/receiving.
 * Supports both blocking and non-blocking receive operations.
 */
class UDP_socket {
    int socketFd; ///< File descriptor for the UDP socket.
    sockaddr_in serverAddr; ///< Server address structure.
    std::unordered_map<std::string, uint32_t> clients; ///< Maps client addresses (as "ip:port") to their assigned IDs.

    public:
        /**
         * @brief Constructs a UDP_socket bound to the specified port.
         * @param port The UDP port to listen on.
         * @throws std::runtime_error if socket creation or binding fails.
         */
        UDP_socket(uint16_t port);

        /**
         * @brief Destroys the UDP_socket and closes the socket file descriptor.
         */
        ~UDP_socket();

        /**
         * @brief Receives a UDP message (blocking).
         * @return A pair containing the received data and the sender's address.
         * @throws std::runtime_error if the receive operation fails.
         */
        std::pair<std::vector<uint8_t>, sockaddr_in> receive();

        /**
         * @brief Attempts to receive a UDP message (non-blocking).
         * @param outData Output parameter for the received data.
         * @param outAddr Output parameter for the sender's address.
         * @return true if a message was received, false otherwise (e.g., no data available).
         */
        bool try_receive(std::vector<uint8_t>& outData, sockaddr_in& outAddr);

        /**
         * @brief Sends a UDP message to a specific client.
         * @param data Pointer to the data to send.
         * @param size Size of the data in bytes.
         * @param clientAddr Destination address.
         */
        void sendTo(const void* data, size_t size, const sockaddr_in& clientAddr);

        /**
         * @brief Broadcasts a UDP message to all connected clients.
         * @param data Pointer to the data to send.
         * @param size Size of the data in bytes.
         */
        void broadcast(const void* data, size_t size);

        /**
         * @brief Registers a new client with the socket.
         * @param clientAddr Address of the client to add.
         * @param clientId Unique ID to assign to the client.
         */
        void addClient(const sockaddr_in& clientAddr, uint32_t clientId);

        /**
         * @brief Gets the number of connected clients.
         * @return Current client count.
         */
        size_t getClientCount() const;

        /**
         * @brief Gets the map of connected clients.
         * @return A const reference to the clients map (address string -> client ID).
         */
        const std::unordered_map<std::string, uint32_t>& getClients() const { return clients; }
};
