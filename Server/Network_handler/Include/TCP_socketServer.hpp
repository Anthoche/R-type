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
 * @brief Simple TCP server socket for JSON communication (1 client).
 */
class TCP_socketServer {
    public:
        TCP_socketServer(uint16_t port);
        ~TCP_socketServer();

        bool acceptClient();
        void disconnectClient();

        void sendJson(const nlohmann::json& j);
        nlohmann::json receiveJson();

    private:
        asio::io_context ioContext;
        asio::ip::tcp::acceptor acceptor;
        asio::ip::tcp::socket socket;
};
