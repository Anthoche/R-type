/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#pragma once

#include "UDP_socket.hpp"
#include "../../../Shared/protocol.hpp"
#include <thread>
#include <atomic>


class GameServer {
    UDP_socket socket;
    std::atomic<bool> gameStarted{false};
    uint32_t nextClientId = 1;

    public:
        GameServer(uint16_t port);
        ~GameServer() = default;

        void run();

    private:
        void handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr);
        void broadcastGameStart();
};
