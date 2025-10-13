/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TCP_socketServer
*/

#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>

/**
 * @class TCP_socketServer
 * @brief Simple TCP server socket for JSON communication (single client).
 *
 * This class manages a TCP server that listens on a specific port and handles
 * a single client connection. It provides methods for accepting a client,
 * sending JSON messages, and receiving JSON messages.
 */
class TCP_socketServer {
    public:
        /**
         * @brief Constructs a TCP_socketServer and binds it to the given port.
         * @param port TCP port to listen on.
         */
        TCP_socketServer(uint16_t port);

        /**
         * @brief Destroys the TCP_socketServer and disconnects the client.
         */
        ~TCP_socketServer();

        /**
         * @brief Waits for a client to connect.
         * @return true if a client successfully connected, false otherwise.
         */
        bool acceptClient();

        /**
         * @brief Disconnects the currently connected client (if any).
         */
        void disconnectClient();

        /**
         * @brief Sends a JSON object to the connected client.
         * @param j JSON object to send.
         */
        void sendJson(const nlohmann::json& j);

        /**
         * @brief Receives a JSON object from the connected client.
         * @return The received JSON object. Returns empty JSON object on error.
         */
        nlohmann::json receiveJson();

    private:
        asio::io_context ioContext;       /**< Internal ASIO I/O context. */
        asio::ip::tcp::acceptor acceptor; /**< TCP acceptor for listening to incoming connections. */
        asio::ip::tcp::socket socket;     /**< TCP socket for communicating with the client. */
};
