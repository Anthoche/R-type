/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#include "server.hpp"
#include "../Game/ServerGame.hpp"
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
    // Delegate to Game module
    ServerGame game(socket);
    game.run();
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

void GameServer::game_loop() {}

void GameServer::initialize_player_positions() {}

void GameServer::process_pending_messages() {}

void GameServer::broadcast_states_to_clients() {}

void GameServer::update_enemies(float dt) {}

void GameServer::spawn_enemy() {}

void GameServer::broadcast_enemy_spawn(uint32_t enemyId, float x, float y) {}

void GameServer::broadcast_enemy_update(uint32_t enemyId, float x, float y) {}

void GameServer::broadcast_enemy_despawn(uint32_t enemyId) {}

void GameServer::initialize_obstacles() {}

void GameServer::broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h) {}

void GameServer::broadcast_obstacle_despawn(uint32_t obstacleId) {}

void GameServer::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}
