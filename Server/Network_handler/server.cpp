/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/

#include "Include/server.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstring>

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
        }
    }
}

void GameServer::game_loop() {
    initialize_player_positions();
    const int tick_ms = 16; // ~60Hz
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        process_pending_messages();
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

void GameServer::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}
