/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGPlayer
*/

#include "../Rungame.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <cmath>

#define RESET_COLOR   "\033[0m"
#define RED_COLOR     "\033[31m"
#define GREEN_COLOR   "\033[32m"
#define YELLOW_COLOR  "\033[33m"
#define BLUE_COLOR    "\033[34m"
#define LOG_ERROR(msg)   std::cerr << RED_COLOR << "[ERROR] " << msg << RESET_COLOR << std::endl
#define LOG_INFO(msg)    std::cout << GREEN_COLOR << "[INFO] " << msg << RESET_COLOR << std::endl
#define LOG_DEBUG(msg)   std::cout << YELLOW_COLOR << "[DEBUG] " << msg << RESET_COLOR << std::endl
#define LOG(msg)         std::cout << BLUE_COLOR << msg << RESET_COLOR << std::endl



void ServerGame::broadcast_player_death(uint32_t clientId) {
    auto recipients = collectRoomClients();
    if (recipients.empty())
        return;

    PlayerDeathMessage msg;
    msg.type = MessageType::PlayerDeath;
    msg.clientId = htonl(clientId);
    connexion.broadcastToClients(recipients, &msg, sizeof(msg));
    LOG_INFO("[Server] Player " << clientId << " died!");
}

void ServerGame::initialize_player_positions() {
    LOG_DEBUG("Initializing player positions...");
    std::vector<uint32_t> clients;
    {
        std::lock_guard<std::mutex> lock(initialClientsMutex);
        for (auto client : initialClients)
            clients.push_back(client.first);
    }

    if (clients.empty()) {
        for (const auto &kv : connexion.getClients())
            clients.push_back(kv.second);
    }

    playerPositions.clear();
    for (std::size_t i = 0; i < clients.size(); ++i) {
        float baseX = 100.f + static_cast<float>(i) * 50.f;
        playerPositions[clients[i]] = {baseX, 300.f};
    }
}