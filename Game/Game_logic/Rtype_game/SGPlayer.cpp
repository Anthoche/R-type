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

void ServerGame::handle_melee_attack(uint32_t attackerId, float range, int damage, float baseKnockback, float knockbackScale, int dirX, int dirY) {
    // Trouver la position de l'attaquant
    auto attackerPosIt = playerPositions.find(attackerId);
    if (attackerPosIt == playerPositions.end()) {
        return;
    }

    float attackerX = attackerPosIt->second.first;
    float attackerY = attackerPosIt->second.second;

    auto &clientIds = registry_server.get_components<component::client_id>();
    auto &drawables = registry_server.get_components<component::drawable>();
    auto &healths = registry_server.get_components<component::health>();

    if (dirX == 0 && dirY == 0) {
        dirX = 1;
        dirY = 0;
    }

    auto get_dimensions_for = [&](uint32_t clientId) {
        float width = 30.f;
        float height = 30.f;
        for (std::size_t i = 0; i < clientIds.size(); ++i) {
            if (clientIds[i] && clientIds[i]->id == clientId) {
                if (i < drawables.size() && drawables[i]) {
                    width = drawables[i]->width;
                    height = drawables[i]->height;
                }
                break;
            }
        }
        return std::pair<float, float>{width, height};
    };

    auto [attackerWidth, attackerHeight] = get_dimensions_for(attackerId);
    float attackerHalfWidth = attackerWidth * 0.5f;
    float attackerHalfHeight = attackerHeight * 0.5f;

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

        float centerDx = targetX - attackerX;
        float centerDy = targetY - attackerY;

        int absDirX = std::abs(dirX);
        int absDirY = std::abs(dirY);
        bool matchesDir = true;
        if (absDirX >= absDirY && absDirX != 0) {
            matchesDir = (dirX > 0) ? (centerDx >= -0.01f) : (centerDx <= 0.01f);
        } else if (absDirY > absDirX) {
            matchesDir = (dirY > 0) ? (centerDy >= -0.01f) : (centerDy <= 0.01f);
        }
        if (!matchesDir)
            continue;

        auto [targetWidth, targetHeight] = get_dimensions_for(targetId);
        float targetHalfWidth = targetWidth * 0.5f;
        float targetHalfHeight = targetHeight * 0.5f;

        // Calculer la distance entre l'attaquant et la cible
        float gapX = std::fabs(targetX - attackerX) - (attackerHalfWidth + targetHalfWidth);
        float gapY = std::fabs(targetY - attackerY) - (attackerHalfHeight + targetHalfHeight);
        gapX = std::max(0.0f, gapX);
        gapY = std::max(0.0f, gapY);
        float distance = std::sqrt(gapX * gapX + gapY * gapY);

        // Si la cible est dans la portée, infliger des dégâts
        if (distance <= range) {
            LOG_INFO("[Server] Player " << attackerId << " hits player " << targetId 
                     << " at distance " << distance << "px for " << damage << " damage");

            // Trouver et réduire les HP du joueur ciblé
            for (std::size_t i = 0; i < healths.size() && i < clientIds.size(); ++i) {
                if (clientIds[i] && clientIds[i]->id == targetId && healths[i]) {
                    healths[i]->current = std::max(0, healths[i]->current - damage);

                    LOG_DEBUG("[Server] Player " << targetId << " health: "
                             << healths[i]->current << "/" << healths[i]->max);

                    float missingRatio = 0.f;
                    if (healths[i]->max > 0) {
                        missingRatio = static_cast<float>(healths[i]->max - healths[i]->current) /
                                       static_cast<float>(healths[i]->max);
                    }
                    float knockMagnitude = baseKnockback + knockbackScale * missingRatio;
                    if (absDirX >= absDirY && absDirX != 0) {
                        float direction = (dirX >= 0) ? 1.f : -1.f;
                        playerKnockbackVelocity[targetId] = direction * knockMagnitude;
                    } else {
                        float direction = (dirY >= 0) ? 1.f : -1.f;
                        playerKnockbackVelocity[targetId] = 0.f;
                        playerVelocities[targetId] = direction * (knockMagnitude * 0.75f);
                    }

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
