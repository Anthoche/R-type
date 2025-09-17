/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#pragma once

#include "../../../Shared/protocol.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <vector>
#include "game_scene.hpp"

class GameClient {
    int socketFd;
    sockaddr_in serverAddr;
    uint32_t clientId = 0;
    std::string clientName;

    public:
        GameClient(const std::string& serverIp, uint16_t serverPort, const std::string& name);
        ~GameClient();

        void run();

    private:
        void sendHello();
        void runGameLoop(uint32_t clientCount);
        void applyPlayerColorByIndex(game::scene::GameScene &scene, uint32_t idx);
        // Helpers (≤25 lignes chacune)
        uint32_t waitForGameStart();
        void gameFrame(game::scene::GameScene &scene, float dt);
        void gatherInput(float &inputX, float &inputY) const;
        void sendInputToServer(float inputX, float inputY) const;
        void readStateUpdates(game::scene::GameScene &scene) const;
};
