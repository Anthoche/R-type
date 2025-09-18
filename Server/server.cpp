/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#include "server.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>
#include <algorithm>

GameServer::GameServer(uint16_t port) : socket(port) {}

void GameServer::run() {
    std::cout << "Serveur démarré. En attente de 4 clients..." << std::endl;

    // accept clients until game start
    while (!gameStarted) {
        std::vector<uint8_t> data;
        sockaddr_in clientAddr = {};
        if (!socket.try_receive(data, clientAddr)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }
        if (data.size() < sizeof(MessageType)) {
            continue;
        }
        MessageType type = *reinterpret_cast<MessageType*>(data.data());
        if (type == MessageType::ClientHello) {
            handleClientHello(data, clientAddr);
        }
    }
    std::cout << "Tous les clients sont connectés. Le jeu commence !" << std::endl;
    game_loop();
}

void GameServer::handleClientHello(const std::vector<uint8_t>& data, const sockaddr_in& clientAddr) {
    if (gameStarted) {
        return;
    }
    if (data.size() < sizeof(ClientHelloMessage)) {
        return;
    }
    const ClientHelloMessage* msg = reinterpret_cast<const ClientHelloMessage*>(data.data());
    uint32_t clientId = nextClientId++;
    socket.addClient(clientAddr, clientId);
    ServerAssignIdMessage assignMsg;
    assignMsg.type = MessageType::ServerAssignId;
    assignMsg.clientId = htonl(clientId);
    socket.sendTo(&assignMsg, sizeof(assignMsg), clientAddr);
    std::cout << "Client connecté : " << msg->clientName
              << " (ID: " << clientId << ")"
              << " [" << socket.getClientCount() << "/4]" << std::endl;
    if (socket.getClientCount() == 4) {
        gameStarted = true;
        broadcastGameStart();
    }
}

void GameServer::broadcastGameStart() {
    GameStartMessage msg;
    msg.type = MessageType::GameStart;
    msg.clientCount = htonl(4);
    socket.broadcast(&msg, sizeof(msg));
    std::cout << "[DEBUG] Message GameStart envoyé à tous les clients." << std::endl;
}

void GameServer::handle_client_message(const std::vector<uint8_t>& data, const sockaddr_in& from) {
    if (data.size() < sizeof(MessageType)) return;
    MessageType type = *reinterpret_cast<const MessageType*>(data.data());
    if (type == MessageType::ClientInput) {
        if (data.size() >= sizeof(ClientInputMessage)) {
            const ClientInputMessage* msg = reinterpret_cast<const ClientInputMessage*>(data.data());
            uint32_t id = ntohl(msg->clientId);
            // decode floats from network order bits
            uint32_t xbits = ntohl(msg->inputXBits);
            uint32_t ybits = ntohl(msg->inputYBits);
            float inputX, inputY;
            std::memcpy(&inputX, &xbits, sizeof(float));
            std::memcpy(&inputY, &ybits, sizeof(float));
            auto& pos = playerPositions[id];
            // simple integration
            float speed = 200.f / 60.f; // units per tick at 60Hz
            pos.first += inputX * speed;
            pos.second += inputY * speed;
            // clamp to window bounds (800x600), player size 30 => half-size 15
            const float halfSize = 15.f;
            const float maxX = 800.f - halfSize;
            const float maxY = 600.f - halfSize;
            pos.first = std::clamp(pos.first, halfSize, maxX);
            pos.second = std::clamp(pos.second, halfSize, maxY);
        }
    }
}

void GameServer::game_loop() {
    initialize_player_positions();
    const int tick_ms = 16; // ~60Hz
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        process_pending_messages();
        update_enemies(1.f / 60.f);
        broadcast_states_to_clients();
        sleep_to_maintain_tick(tick_start, tick_ms);
    }
}

void GameServer::initialize_player_positions() {
    for (const auto& kv : socket.getClients()) {
        playerPositions[kv.second] = {100.f + (kv.second - 1) * 50.f, 300.f};
    }
}

void GameServer::process_pending_messages() {
    while (true) {
        std::vector<uint8_t> data;
        sockaddr_in from = {};
        if (!socket.try_receive(data, from)) break;
        handle_client_message(data, from);
    }
}

void GameServer::broadcast_states_to_clients() {
    for (const auto& kv : socket.getClients()) {
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
        socket.broadcast(&m, sizeof(m));
    }
}

void GameServer::update_enemies(float dt) {
    // spawn every 2 seconds
    enemySpawnTimerSec += dt;
    if (enemySpawnTimerSec >= 2.f) {
        enemySpawnTimerSec = 0.f;
        spawn_enemy();
    }
    // move enemies left, despawn if off-screen
    std::vector<uint32_t> toDespawn;
    for (auto &kv : enemies) {
        kv.second.first -= 100.f * dt; // x -= 100 px/s
        if (kv.second.first < -50.f) {
            toDespawn.push_back(kv.first);
        } else {
            broadcast_enemy_update(kv.first, kv.second.first, kv.second.second);
        }
    }
    for (uint32_t id : toDespawn) {
        broadcast_enemy_despawn(id);
        enemies.erase(id);
    }
}

void GameServer::spawn_enemy() {
    uint32_t id = nextEnemyId++;
    float x = 800.f - 50.f;
    // simple deterministic lane pattern
    float y = 100.f + 100.f * ((id - 1) % 4);
    enemies[id] = {x, y};
    broadcast_enemy_spawn(id, x, y);
}

void GameServer::broadcast_enemy_spawn(uint32_t enemyId, float x, float y) {
    EnemySpawnMessage m;
    m.type = MessageType::EnemySpawn;
    m.enemyId = htonl(enemyId);
    uint32_t xbits, ybits;
    std::memcpy(&xbits, &x, sizeof(float));
    std::memcpy(&ybits, &y, sizeof(float));
    m.posXBits = htonl(xbits);
    m.posYBits = htonl(ybits);
    socket.broadcast(&m, sizeof(m));
}

void GameServer::broadcast_enemy_update(uint32_t enemyId, float x, float y) {
    EnemyUpdateMessage m;
    m.type = MessageType::EnemyUpdate;
    m.enemyId = htonl(enemyId);
    uint32_t xbits, ybits;
    std::memcpy(&xbits, &x, sizeof(float));
    std::memcpy(&ybits, &y, sizeof(float));
    m.posXBits = htonl(xbits);
    m.posYBits = htonl(ybits);
    socket.broadcast(&m, sizeof(m));
}

void GameServer::broadcast_enemy_despawn(uint32_t enemyId) {
    EnemyDespawnMessage m;
    m.type = MessageType::EnemyDespawn;
    m.enemyId = htonl(enemyId);
    socket.broadcast(&m, sizeof(m));
}

void GameServer::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}
