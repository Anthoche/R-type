/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGPlayer
*/

#include "../Rungame.hpp"
#include "../../../Shared/Logger.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cstring>
#include <thread>
#include <fstream>
#include <iostream>
#include <cmath>



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