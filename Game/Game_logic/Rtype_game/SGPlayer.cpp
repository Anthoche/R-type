/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGPlayer
*/

#include "../Rtype_game.hpp"
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
    PlayerDeathMessage msg;
    msg.type = MessageType::PlayerDeath;
    msg.clientId = htonl(clientId);
    connexion.broadcast(&msg, sizeof(msg));
    LOG_INFO("[Server] Player " << clientId << " died!");
}

void ServerGame::initialize_player_positions() {
    LOG_DEBUG("Initializing player positions...");
    for (const auto& kv : connexion.getClients()) {
        playerPositions[kv.second] = {300.f + (kv.second - 1) * 50.f, 300.f};
    }
}

void ServerGame::handle_melee_attack(uint32_t attackerId, float range, int damage) {
    // Trouver la position de l'attaquant
    auto attackerPosIt = playerPositions.find(attackerId);
    if (attackerPosIt == playerPositions.end()) {
        return;
    }

    float attackerX = attackerPosIt->second.first;
    float attackerY = attackerPosIt->second.second;

    LOG_DEBUG("[Server] Player " << attackerId << " performs melee attack (range=" << range << ", damage=" << damage << ")");

    // Vérifier tous les autres joueurs
    for (auto &kv : playerPositions) {
        uint32_t targetId = kv.first;
        
        // Ignorer l'attaquant lui-même et les joueurs déjà morts
        if (targetId == attackerId || deadPlayers.find(targetId) != deadPlayers.end()) {
            continue;
        }

        float targetX = kv.second.first;
        float targetY = kv.second.second;

        // Calculer la distance entre l'attaquant et la cible
        float dx = targetX - attackerX;
        float dy = targetY - attackerY;
        float distance = std::sqrt(dx * dx + dy * dy);

        // Si la cible est dans la portée, infliger des dégâts
        if (distance <= range) {
            LOG_INFO("[Server] Player " << attackerId << " hits player " << targetId 
                     << " at distance " << distance << "px for " << damage << " damage");

            // Trouver et réduire les HP du joueur ciblé
            auto& healths = registry_server.get_components<component::health>();
            auto& clientIds = registry_server.get_components<component::client_id>();

            for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
                if (clientIds[i] && clientIds[i]->id == targetId && healths[i]) {
                    healths[i]->current = std::max(0, healths[i]->current - damage);
                    
                    LOG_DEBUG("[Server] Player " << targetId << " health: " 
                             << healths[i]->current << "/" << healths[i]->max);

                    // Vérifier si le joueur est mort
                    if (healths[i]->current <= 0) {
                        LOG_INFO("[Server] Player " << targetId << " was killed by player " << attackerId);
                        deadPlayers.insert(targetId);
                        broadcast_player_death(targetId);
                    }
                    
                    break;
                }
            }
        }
    }
}