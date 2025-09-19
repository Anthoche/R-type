/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** game_core
*/

#include "Include/game_core.hpp"
#include <iostream>
#include <thread>
#include <chrono>


GameCore::GameCore(Connexion& connexion) : connexion(connexion) {}

void GameCore::run() {
    initializePlayerPositions();
    const int tick_ms = 16; // ~60Hz
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        processPendingMessages();
        updateEnemies(1.f / 60.f);
        broadcastStatesToClients();
        sleepToMaintainTick(tick_start, tick_ms);
    }
}

void GameCore::initializePlayerPositions() {
    for (const auto& kv : connexion.getClients()) {
        playerPositions[kv.second] = {100.f + (kv.second - 1) * 50.f, 300.f};
    }
}

void GameCore::processPendingMessages() {
    while (true) {
        std::vector<uint8_t> data;
        sockaddr_in from = {};
        if (!connexion.tryReceive(data, from)) break;
        handleClientMessage(data);
    }
}

void GameCore::handleClientMessage(const std::vector<uint8_t>& data) {
    if (data.size() < sizeof(MessageType)) return;
    MessageType type = *reinterpret_cast<const MessageType*>(data.data());
    if (type == MessageType::ClientInput) {
        if (data.size() >= sizeof(ClientInputMessage)) {
            const ClientInputMessage* msg = reinterpret_cast<const ClientInputMessage*>(data.data());
            uint32_t id = ntohl(msg->clientId);
            uint32_t xbits = ntohl(msg->inputXBits);
            uint32_t ybits = ntohl(msg->inputYBits);
            float inputX, inputY;
            std::memcpy(&inputX, &xbits, sizeof(float));
            std::memcpy(&inputY, &ybits, sizeof(float));
            auto& pos = playerPositions[id];
            float speed = 200.f / 60.f;
            pos.first += inputX * speed;
            pos.second += inputY * speed;
            const float halfSize = 15.f;
            const float maxX = 800.f - halfSize;
            const float maxY = 600.f - halfSize;
            if (pos.first < halfSize) pos.first = halfSize;
            if (pos.first > maxX) pos.first = maxX;
            if (pos.second < halfSize) pos.second = halfSize;
            if (pos.second > maxY) pos.second = maxY;
        }
    }
}

void GameCore::broadcastStatesToClients() {
    for (const auto& kv : connexion.getClients()) {
        uint32_t id = kv.second;
        auto it = playerPositions.find(id);
        if (it == playerPositions.end()) continue;
        StateUpdateMessage m;
        m.type = MessageType::StateUpdate;
        m.clientId = htonl(id);
        uint32_t xbits, ybits;
        std::memcpy(&xbits, &it->second.first, sizeof(float));
        std::memcpy(&ybits, &it->second.second, sizeof(float));
        m.posXBits = htonl(xbits);
        m.posYBits = htonl(ybits);
        connexion.broadcast(&m, sizeof(m));
    }
}

void GameCore::updateEnemies(float dt) {
    enemySpawnTimerSec += dt;
    if (enemySpawnTimerSec >= 2.f) {
        enemySpawnTimerSec = 0.f;
        spawnEnemy();
    }
    std::vector<uint32_t> toDespawn;
    for (auto &kv : enemies) {
        kv.second.first -= 100.f * dt;
        if (kv.second.first < -50.f) {
            toDespawn.push_back(kv.first);
        } else {
            broadcastEnemyUpdate(kv.first, kv.second.first, kv.second.second);
        }
    }
    for (uint32_t id : toDespawn) {
        broadcastEnemyDespawn(id);
        enemies.erase(id);
    }
}

void GameCore::spawnEnemy() {
    uint32_t id = nextEnemyId++;
    float x = 800.f - 50.f;
    float y = 100.f + 100.f * ((id - 1) % 4);
    enemies[id] = {x, y};
    broadcastEnemySpawn(id, x, y);
}

void GameCore::broadcastEnemySpawn(uint32_t enemyId, float x, float y) {
    EnemySpawnMessage m;
    m.type = MessageType::EnemySpawn;
    m.enemyId = htonl(enemyId);
    uint32_t xbits, ybits;
    std::memcpy(&xbits, &x, sizeof(float));
    std::memcpy(&ybits, &y, sizeof(float));
    m.posXBits = htonl(xbits);
    m.posYBits = htonl(ybits);
    connexion.broadcast(&m, sizeof(m));
}

void GameCore::broadcastEnemyUpdate(uint32_t enemyId, float x, float y) {
    EnemyUpdateMessage m;
    m.type = MessageType::EnemyUpdate;
    m.enemyId = htonl(enemyId);
    uint32_t xbits, ybits;
    std::memcpy(&xbits, &x, sizeof(float));
    std::memcpy(&ybits, &y, sizeof(float));
    m.posXBits = htonl(xbits);
    m.posYBits = htonl(ybits);
    connexion.broadcast(&m, sizeof(m));
}

void GameCore::broadcastEnemyDespawn(uint32_t enemyId) {
    EnemyDespawnMessage m;
    m.type = MessageType::EnemyDespawn;
    m.enemyId = htonl(enemyId);
    connexion.broadcast(&m, sizeof(m));
}

void GameCore::sleepToMaintainTick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}
