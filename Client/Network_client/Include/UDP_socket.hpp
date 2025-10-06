/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** UDP_socket client (robuste avec gestion d'erreurs)
*/

#pragma once

#include <asio.hpp>
#include <vector>
#include <array>
#include <utility>

/**
 * @brief UDP socket wrapper for cross-platform client network communication.
 *
 * Manages a UDP socket for client-side network operations.
 * Provides a unified interface for both Windows and Linux.
 */
class UDP_socket {
    public:
        /**
         * @brief Constructs a UDP_socket.
         * @throws std::runtime_error if socket creation fails.
         */
        UDP_socket();

        /**
         * @brief Destroys the UDP_socket and closes the socket.
         */
        ~UDP_socket();

        /**
         * @brief Attempts to receive data from the socket without blocking.
         * @param outData Vector to store received data.
         * @param outEndpoint Endpoint of the sender.
         * @return true if data was received, false if no data is available.
         */
        bool try_receive(std::vector<uint8_t>& outData, asio::ip::udp::endpoint& outEndpoint);

        /**
         * @brief Receives data from the socket, blocking until data is available.
         * @return A pair containing the received data vector and the sender's endpoint.
         * @throws std::runtime_error if receiving fails.
         */
        std::pair<std::vector<uint8_t>, asio::ip::udp::endpoint> receive();

        /**
         * @brief Sends data to the specified endpoint.
         * @param data Pointer to the data to send.
         * @param size Size of the data in bytes.
         * @param endpoint Destination endpoint.
         * @throws std::runtime_error if sending fails.
         */
        void sendTo(const void* data, size_t size, const asio::ip::udp::endpoint& endpoint);

    private:
        asio::io_context ioContext;
        asio::ip::udp::socket socket;
};
