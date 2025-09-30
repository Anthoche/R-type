/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/
#include "Include/ServerGame.hpp"
#include <algorithm>
#include <cstring>
#include <thread>
#include <iostream>

ServerGame::ServerGame(Connexion &conn) : connexion(conn) {}

void ServerGame::run() {
    initialize_player_positions();
    initialize_obstacles();
    setup_async_receive();
    const int tick_ms = 16;
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        broadcast_states_to_clients();
        sleep_to_maintain_tick(tick_start, tick_ms);
    }
}

void ServerGame::initialize_player_positions() {
    for (const auto& kv : connexion.getClients()) {
        playerPositions[kv.second] = {100.f + (kv.second - 1) * 50.f, 300.f};
    }
}

void ServerGame::setup_async_receive() {
    connexion.asyncReceive(
        [this](const asio::error_code& ec, std::vector<uint8_t> data, asio::ip::udp::endpoint from) {
            if (!ec && !data.empty()) {
                handle_client_message(data, from);
            }
            setup_async_receive();
        }
    );
}

void ServerGame::handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from) {
    if (data.size() < sizeof(MessageType)) return;
    MessageType type = *reinterpret_cast<const MessageType*>(data.data());
    if (type == MessageType::ClientInput && data.size() >= sizeof(ClientInputMessage)) {
        const ClientInputMessage* msg = reinterpret_cast<const ClientInputMessage*>(data.data());
        uint32_t id = ntohl(msg->clientId);
        uint32_t xbits = ntohl(msg->inputXBits);
        uint32_t ybits = ntohl(msg->inputYBits);
        float inputX, inputY;
        std::memcpy(&inputX, &xbits, sizeof(float));
        std::memcpy(&inputY, &ybits, sizeof(float));
        auto& pos = playerPositions[id];
        float speed = 200.f / 60.f; // units per tick at 60Hz
        pos.first += inputX * speed;
        pos.second += inputY * speed;
        const float halfSize = 15.f;
        const float maxX = 800.f - halfSize;
        const float maxY = 600.f - halfSize;
        pos.first = std::clamp(pos.first, halfSize, maxX);
        pos.second = std::clamp(pos.second, halfSize, maxY);
    }
}

void ServerGame::broadcast_states_to_clients() {
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

void ServerGame::initialize_obstacles() {
    struct O { float x,y,w,h; };
    O list[3] = {
        {200.f, 400.f, 60.f, 60.f},
        {400.f, 400.f, 60.f, 60.f},
        {600.f, 400.f, 60.f, 60.f}
    };
    uint32_t oid = 1;
    for (auto &o : list) {
        obstacles[oid] = std::make_tuple(o.x, o.y, o.w, o.h);
        broadcast_obstacle_spawn(oid, o.x, o.y, o.w, o.h);
        ++oid;
    }
}

void ServerGame::broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h) {
    ObstacleSpawnMessage m;
    m.type = MessageType::ObstacleSpawn;
    m.obstacleId = htonl(obstacleId);
    uint32_t xb, yb, wb, hb;
    std::memcpy(&xb, &x, sizeof(float));
    std::memcpy(&yb, &y, sizeof(float));
    std::memcpy(&wb, &w, sizeof(float));
    std::memcpy(&hb, &h, sizeof(float));
    m.posXBits = htonl(xb);
    m.posYBits = htonl(yb);
    m.widthBits = htonl(wb);
    m.heightBits = htonl(hb);
    connexion.broadcast(&m, sizeof(m));
}

void ServerGame::broadcast_obstacle_despawn(uint32_t obstacleId) {
    ObstacleDespawnMessage m;
    m.type = MessageType::ObstacleDespawn;
    m.obstacleId = htonl(obstacleId);
    connexion.broadcast(&m, sizeof(m));
}

void ServerGame::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}