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
 * @brief Simple TCP client socket for JSON communication.
 */
class TCP_socketClient {
    public:
        TCP_socketClient();
        ~TCP_socketClient();

        bool connectToServer(const std::string& host, uint16_t port);
        void disconnect();

        void sendJson(const nlohmann::json& j);
        nlohmann::json receiveJson();

    private:
        asio::io_context ioContext;
        asio::ip::tcp::socket socket;
};
