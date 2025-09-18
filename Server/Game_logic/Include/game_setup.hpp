/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** game_setup
*/

#pragma once

#include "connexion.hpp"
#include "protocol.hpp"
#include <atomic>


class GameSetup {
    public:
        GameSetup(Connexion& connexion);
        ~GameSetup() = default;

        bool waitForPlayers();
        void broadcastGameStart();

    private:
        Connexion& connexion;
        std::atomic<uint32_t> nextClientId{1};
        std::atomic<bool> gameStarted{false};

        void handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr);
};
