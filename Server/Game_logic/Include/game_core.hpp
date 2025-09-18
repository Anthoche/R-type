/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** game_core
*/

#pragma once

#include "connexion.hpp"
#include "protocol.hpp"
#include <unordered_map>
#include <chrono>


class GameCore {
    public:
        GameCore(Connexion& connexion);
        ~GameCore() = default;

        void run();

    private:
        Connexion& connexion;
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;
        std::unordered_map<uint32_t, std::pair<float, float>> enemies;
        uint32_t nextEnemyId = 1;
        float enemySpawnTimerSec = 0.f;

        void initializePlayerPositions();
        void processPendingMessages();
        void broadcastStatesToClients();
        void updateEnemies(float dt);
        void spawnEnemy();
        void broadcastEnemySpawn(uint32_t enemyId, float x, float y);
        void broadcastEnemyUpdate(uint32_t enemyId, float x, float y);
        void broadcastEnemyDespawn(uint32_t enemyId);
        void sleepToMaintainTick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
        void handleClientMessage(const std::vector<uint8_t>& data);
};
