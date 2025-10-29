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
#include <limits>

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
        float spawnX = 300.f + (kv.second - 1) * 50.f;
        float spawnY = 300.f;
        playerPositions[kv.second] = {spawnX, spawnY};
        playerSpawnPositions[kv.second] = {spawnX, spawnY};
        ensure_player_tracking(kv.second);
        playerLives[kv.second] = 3;
        playerVelocities[kv.second] = 0.f;
        playerHorizontalKnockback[kv.second] = 0.f;
        playerVerticalKnockback[kv.second] = 0.f;
        playerJumpCount[kv.second] = 0;
        playerInputStates[kv.second] = PlayerInputState{};
        deadPlayers.erase(kv.second);
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

    struct PlayerInfo {
        float width;
        float height;
        std::size_t index;
    };

    auto get_info_for = [&](uint32_t clientId) {
        PlayerInfo info{30.f, 30.f, clientIds.size()};
        for (std::size_t i = 0; i < clientIds.size(); ++i) {
            if (clientIds[i] && clientIds[i]->id == clientId) {
                if (i < drawables.size() && drawables[i]) {
                    info.width = drawables[i]->width;
                    info.height = drawables[i]->height;
                }
                info.index = i;
                break;
            }
        }
        return info;
    };

    auto attackerInfo = get_info_for(attackerId);
    float attackerWidth = attackerInfo.width;
    float attackerHeight = attackerInfo.height;
    float attackerHalfWidth = attackerWidth * 0.5f;
    float attackerHalfHeight = attackerHeight * 0.5f;

    LOG_DEBUG("[Server] Player " << attackerId << " performs melee attack (range=" << range << ", damage=" << damage << ")");

    // Vérifier tous les autres joueurs
    uint32_t bestTargetId = 0;
    std::size_t bestIndex = clientIds.size();
    float bestDistance = std::numeric_limits<float>::max();
    bool bestHorizontal = true;
    bool foundTarget = false;

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
        bool horizontalAxis = (absDirX >= absDirY);
        float primaryComponent = horizontalAxis ? centerDx : centerDy;
        float axisSign = (absDirX >= absDirY) ? static_cast<float>(dirX) : static_cast<float>(dirY);
        if (axisSign > 0.f && primaryComponent < 0.f)
            continue;
        if (axisSign < 0.f && primaryComponent > 0.f)
            continue;

        PlayerInfo info = get_info_for(targetId);
        if (info.index == clientIds.size())
            continue;
        float targetHalfWidth = info.width * 0.5f;
        float targetHalfHeight = info.height * 0.5f;

        // Calculer la distance entre l'attaquant et la cible
        float gapX = std::fabs(targetX - attackerX) - (attackerHalfWidth + targetHalfWidth);
        float gapY = std::fabs(targetY - attackerY) - (attackerHalfHeight + targetHalfHeight);
        gapX = std::max(0.0f, gapX);
        gapY = std::max(0.0f, gapY);
        float distance = std::sqrt(gapX * gapX + gapY * gapY);

        // Si la cible est dans la portée, infliger des dégâts
        if (distance <= range && distance < bestDistance) {
            bestDistance = distance;
            bestTargetId = targetId;
            bestIndex = info.index;
            bestHorizontal = horizontalAxis;
            foundTarget = true;
        }
    }

    if (foundTarget && bestIndex < healths.size() && healths[bestIndex]) {
        LOG_INFO("[Server] Player " << attackerId << " hits player " << bestTargetId
                 << " at distance " << bestDistance << "px for " << damage << " damage");

        healths[bestIndex]->current = std::max(0, healths[bestIndex]->current - damage);

        LOG_DEBUG("[Server] Player " << bestTargetId << " health: "
                 << healths[bestIndex]->current << "/" << healths[bestIndex]->max);

        float missingRatio = 0.f;
        if (healths[bestIndex]->max > 0) {
            missingRatio = static_cast<float>(healths[bestIndex]->max - healths[bestIndex]->current) /
                           static_cast<float>(healths[bestIndex]->max);
        }
        float knockMagnitude = baseKnockback + knockbackScale * missingRatio;
        if (bestHorizontal) {
            float direction = (dirX >= 0) ? 1.f : -1.f;
            playerHorizontalKnockback[bestTargetId] = direction * knockMagnitude;
            if (baseKnockback >= 650.f) {
                playerVerticalKnockback[bestTargetId] = -knockMagnitude * 0.35f;
            } else {
                playerVerticalKnockback[bestTargetId] = 0.f;
            }
        } else {
            float direction = (dirY >= 0) ? 1.f : -1.f;
            playerHorizontalKnockback[bestTargetId] = 0.f;
            playerVerticalKnockback[bestTargetId] = direction * (knockMagnitude * 0.75f);
        }

        if (healths[bestIndex]->current <= 0) {
            LOG_INFO("[Server] Player " << bestTargetId << " was defeated by player " << attackerId);
            handle_player_defeat(bestTargetId, bestIndex);
        }
    }
}
