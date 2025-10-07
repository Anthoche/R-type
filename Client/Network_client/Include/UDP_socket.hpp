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
 * @class UDP_socket
 * @brief Cross-platform UDP socket wrapper for client-side communication.
 *
 * This class provides a simple and robust wrapper around ASIO’s UDP socket.
 * It supports both blocking and non-blocking operations, with built-in error handling.
 * Designed for lightweight client network communication in real-time applications.
 */
class UDP_socket {
    public:
        /**
         * @brief Constructs a UDP_socket instance.
         *
         * Opens a UDP socket in non-blocking mode using IPv4.
         * @throws std::runtime_error if socket initialization fails.
         */
        UDP_socket();

        /**
         * @brief Destroys the UDP_socket and closes its socket safely.
         */
        ~UDP_socket();

        /**
         * @brief Attempts to receive data from the socket without blocking.
         *
         * If no data is available, the function returns false instead of blocking.
         *
         * @param outData A vector that will contain the received bytes.
         * @param outEndpoint The endpoint of the sender (output parameter).
         * @return true if data was successfully received, false if no data is available.
         *
         * @warning This method does not wait for incoming data.
         */
        bool try_receive(std::vector<uint8_t>& outData, asio::ip::udp::endpoint& outEndpoint);

        /**
         * @brief Receives data from the socket (blocking).
         *
         * Waits until data is available, then returns the received bytes and sender’s endpoint.
         *
         * @return A pair containing:
         *  - A vector with the received bytes.
         *  - The sender's UDP endpoint.
         *
         * @throws std::runtime_error if receiving data fails.
         */
        std::pair<std::vector<uint8_t>, asio::ip::udp::endpoint> receive();

        /**
         * @brief Sends data to a specific remote endpoint.
         *
         * @param data Pointer to the data buffer to send.
         * @param size Size of the data buffer in bytes.
         * @param endpoint The destination UDP endpoint.
         *
         * @throws std::runtime_error if the send operation fails.
         */
        void sendTo(const void* data, size_t size, const asio::ip::udp::endpoint& endpoint);

    private:
        asio::io_context ioContext;   /**< Internal ASIO I/O context used by the socket. */
        asio::ip::udp::socket socket; /**< UDP socket instance for data transmission. */
};
