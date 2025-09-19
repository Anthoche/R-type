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
#include <mutex>
#include <unordered_map>
#include <atomic>
#include <utility>
#include "../../../Engine/Rendering/Raylib.hpp"


class GameClient {
    int socketFd;
    sockaddr_in serverAddr;
    uint32_t clientId{0};
    std::string clientName;
    std::thread rxThread;
    std::atomic<bool> running{false};
    std::atomic<bool> gameStarted{false};
    std::mutex stateMutex;
    std::unordered_map<uint32_t, std::pair<float,float>> players; // includes self by id
    std::unordered_map<uint32_t, std::pair<float,float>> enemies; // id -> (x,y)
    std::unordered_map<uint32_t, std::tuple<float,float,float,float>> obstacles; // id -> (x,y,w,h)
    Raylib raylib;

    public:
        GameClient(const std::string& serverIp, uint16_t serverPort, const std::string& name);
        ~GameClient();
        void run();
        void runRenderLoop();

    private:
        void sendHello();
        void recvLoop();
        void sendInput(float inputX, float inputY);

        // Message handling
        void handleMessage(MessageType type, const std::vector<uint8_t>& buffer);

        // Player-related
        void handleServerAssignId(const std::vector<uint8_t>& buffer);
        void handleGameStart(const std::vector<uint8_t>& buffer);
        void handlePlayerUpdate(const std::vector<uint8_t>& buffer);

        // Enemy-related
        void handleEnemySpawn(const std::vector<uint8_t>& buffer);
        void handleEnemyUpdate(const std::vector<uint8_t>& buffer);
        void handleEnemyDespawn(const std::vector<uint8_t>& buffer);

        // Obstacle-related
        void handleObstacleSpawn(const std::vector<uint8_t>& buffer);
        void handleObstacleDespawn(const std::vector<uint8_t>& buffer);
};
