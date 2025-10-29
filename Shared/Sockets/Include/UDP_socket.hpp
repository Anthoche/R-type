/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** UDP_socket (Unified Client/Server)
*/

#pragma once

#include <asio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <cstdint>
#include <iostream>

/**
 * @class UDP_socket
 * @brief Cross-platform UDP socket wrapper for client/server communication.
 *
 * Unified implementation supporting both client and server modes.
 * Provides methods for sending, receiving, broadcasting messages,
 * and managing multiple clients (server mode only).
 * Supports both blocking and non-blocking operations.
 */
class UDP_socket {
    public:
        /**
         * @brief Constructs a UDP socket in client mode (no binding).
         * @throws std::runtime_error if socket creation fails.
         */
        UDP_socket();

        /**
         * @brief Constructs a UDP socket in server mode and binds it to the given port.
         * @param port Port number to bind the UDP socket.
         * @throws std::runtime_error if socket creation or binding fails.
         */
        explicit UDP_socket(uint16_t port);

        /**
         * @brief Destroys the UDP_socket and closes the socket.
         */
        ~UDP_socket();

        /**
         * @brief Receives a UDP message in blocking mode.
         * @return A pair containing the received data vector and the sender's endpoint.
         * @throws std::runtime_error on receive errors.
         */
        std::pair<std::vector<uint8_t>, asio::ip::udp::endpoint> receive();

        /**
         * @brief Attempts to receive a UDP message without blocking.
         * @param outData Vector to store received bytes.
         * @param outEndpoint Sender's endpoint (output parameter).
         * @return true if data was received, false if no data is available.
         */
        bool try_receive(std::vector<uint8_t>& outData, asio::ip::udp::endpoint& outEndpoint);

        /**
         * @brief Sends a UDP message to the specified endpoint.
         * @param data Pointer to the data buffer.
         * @param size Size of the data in bytes.
         * @param endpoint Destination UDP endpoint.
         */
        void sendTo(const void* data, size_t size, const asio::ip::udp::endpoint& endpoint);

        /**
         * @brief Broadcasts a message to all registered clients (server mode only).
         * @param data Pointer to the data buffer.
         * @param size Size of the data in bytes.
         */
        void broadcast(const void* data, size_t size);

		/**
		* @brief Broadcasts a UDP message to specified clients.
		* @param roomClients The clients to broadcast.
		* @param data Pointer to the data to broadcast.
		* @param size Size of the data in bytes.
		*/
        void broadcastToClients(std::vector<uint32_t> const &roomClients, const void* data, size_t size);

        /**
         * @brief Registers a client with a unique ID (server mode only).
         * @param endpoint Client's UDP endpoint.
         * @param clientId Unique client ID.
         */
        void addClient(const asio::ip::udp::endpoint& endpoint, uint32_t clientId);

        /**
         * @brief Disconnects a client by removing it from the list (server mode only).
         * @param clientId ID of the client to disconnect.
         */
        void disconnectClient(uint32_t clientId);

        /**
         * @brief Returns the number of currently connected clients (server mode only).
         * @return Number of clients.
         */
        size_t getClientCount() const;

        /**
         * @brief Returns a map of client addresses to their IDs (server mode only).
         * @return const reference to the clients map.
         */
        const std::unordered_map<std::string, uint32_t>& getClients() const { return clients; }

    private:
        asio::io_context ioContext;                           /**< Internal ASIO I/O context. */
        asio::ip::udp::socket socket;                        /**< UDP socket instance. */
        std::unordered_map<std::string, uint32_t> clients;  /**< Maps client address strings to client IDs. */
        mutable std::mutex clientsMutex;                     /**< Mutex for thread-safe client map access. */
        bool isServerMode;                                   /**< Flag to track if socket is in server mode. */
};