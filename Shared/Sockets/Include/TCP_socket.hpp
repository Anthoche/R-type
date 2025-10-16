/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TCP_socket (Unified Client/Server)
*/

#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <memory>

/**
 * @class TCP_socket
 * @brief Cross-platform TCP socket wrapper for client/server communication.
 *
 * Unified implementation supporting both client and server modes.
 * Provides methods for connecting, accepting, sending and receiving JSON messages.
 * Uses nlohmann::json for message serialization.
 */
class TCP_socket {
    public:
        /**
         * @brief Constructs a TCP socket in client mode.
         * 
         * Creates a client socket ready to connect to a server.
         */
        TCP_socket();

        /**
         * @brief Constructs a TCP socket in server mode and binds it to the given port.
         * 
         * Creates a server socket that listens on the specified port.
         * 
         * @param port TCP port to listen on.
         * @throws std::runtime_error if acceptor initialization fails.
         */
        explicit TCP_socket(uint16_t port);

        /**
         * @brief Destroys the TCP_socket and closes all connections.
         */
        ~TCP_socket();

        /**
         * @brief Connects the client to a TCP server (client mode only).
         * 
         * @param host The hostname or IP address of the server.
         * @param port The port number to connect to.
         * @return true if the connection was successful, false otherwise.
         */
        bool connectToServer(const std::string& host, uint16_t port);

        /**
         * @brief Waits for a client to connect (server mode only).
         * 
         * Blocks until a client successfully connects to the server.
         * 
         * @return true if a client successfully connected, false otherwise.
         */
        bool acceptClient();

        /**
         * @brief Disconnects the current connection.
         * 
         * For client mode: disconnects from the server.
         * For server mode: disconnects the current client.
         */
        void disconnect();

        /**
         * @brief Sends a JSON object through the TCP connection.
         * 
         * The JSON object is serialized and sent with a newline delimiter.
         * 
         * @param j The JSON object to send.
         */
        void sendJson(const nlohmann::json& j);

        /**
         * @brief Receives a JSON object from the TCP connection.
         * 
         * Waits for a complete JSON message (terminated by a newline).
         * 
         * @return The received JSON object.
         * Returns an empty JSON object if an error occurs.
         */
        nlohmann::json receiveJson();

        /**
         * @brief Checks if the socket is currently connected.
         * 
         * @return true if the socket is open and connected, false otherwise.
         */
        bool isConnected() const;

    private:
        asio::io_context ioContext;                           /**< Internal ASIO I/O context. */
        asio::ip::tcp::socket socket;                        /**< TCP socket for communication. */
        std::unique_ptr<asio::ip::tcp::acceptor> acceptor;  /**< TCP acceptor (server mode only). */
        bool isServerMode;                                   /**< Flag to track if socket is in server mode. */
};