/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TCP_socketClient
*/

#pragma once

#include <asio.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <iostream>

/**
 * @class TCP_socketClient
 * @brief Simple TCP client socket for sending and receiving JSON data.
 *
 * This class provides basic functionalities to connect to a TCP server,
 * send JSON messages, and receive JSON responses using ASIO and nlohmann::json.
 */
class TCP_socketClient {
    public:
        /**
         * @brief Constructs a new TCP_socketClient object.
         *
         * Initializes the TCP socket with an internal io_context.
         */
        TCP_socketClient();

        /**
         * @brief Destroys the TCP_socketClient object.
         *
         * Automatically disconnects the client if still connected.
         */
        ~TCP_socketClient();

        /**
         * @brief Connects the client to a TCP server.
         *
         * @param host The hostname or IP address of the server.
         * @param port The port number to connect to.
         * @return true if the connection was successful, false otherwise.
         */
        bool connectToServer(const std::string& host, uint16_t port);

        /**
         * @brief Disconnects the client from the server.
         *
         * Closes the socket if it is currently open.
         */
        void disconnect();

        /**
         * @brief Sends a JSON object to the connected server.
         *
         * The JSON object is serialized and sent through the TCP socket.
         *
         * @param j The JSON object to send.
         */
        void sendJson(const nlohmann::json& j);

        /**
         * @brief Receives a JSON object from the server.
         *
         * Waits for a complete JSON message (terminated by a newline).
         *
         * @return The received JSON object.
         * Returns an empty JSON object if an error occurs.
         */
        nlohmann::json receiveJson();

    private:
        asio::io_context ioContext;   /**< Internal ASIO I/O context. */
        asio::ip::tcp::socket socket; /**< TCP socket used for communication. */
};
