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
#include <fstream>
#include <iostream>
#include <cmath>


ServerGame::ServerGame(UDP_socket &sock) : socket(sock), registry_server() {}

void ServerGame::run() {
    initialize_player_positions();
    initialize_obstacles();
    const int tick_ms = 16;
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        process_pending_messages();
        broadcast_states_to_clients();
        sleep_to_maintain_tick(tick_start, tick_ms);
    }
}

void ServerGame::initialize_player_positions() {
    for (const auto& kv : socket.getClients()) {
        playerPositions[kv.second] = {100.f + (kv.second - 1) * 50.f, 300.f};
    }
}

void ServerGame::process_pending_messages() {
    while (true) {
        std::vector<uint8_t> data;
        sockaddr_in from = {};
        if (!socket.try_receive(data, from)) break;
        handle_client_message(data, from);
    }
}

bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                        float left2, float right2, float top2, float bottom2) {
    return right1 > left2 && left1 < right2 && bottom1 > top2 && top1 < bottom2;
}

bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                        const std::unordered_map<uint32_t, std::tuple<float, float, float, float>>& obstacles) {
    float halfW = playerWidth * 0.5f;
    float halfH = playerHeight * 0.5f;
    float left   = testX - halfW;
    float right  = testX + halfW;
    float top    = testY - halfH;
    float bottom = testY + halfH;
    
    for (const auto& kv : obstacles) {
        float obsX = std::get<0>(kv.second);
        float obsY = std::get<1>(kv.second);
        float obsW = std::get<2>(kv.second);
        float obsH = std::get<3>(kv.second);
        
        float obsLeft   = obsX - obsW * 0.5f;
        float obsRight  = obsX + obsW * 0.5f;
        float obsTop    = obsY - obsH * 0.5f;
        float obsBottom = obsY + obsH * 0.5f;
        
        if (check_aabb_overlap(left, right, top, bottom,
                              obsLeft, obsRight, obsTop, obsBottom)) {
            return true;
        }
    }
    return false;
}

void ServerGame::handle_client_message(const std::vector<uint8_t>& data, const sockaddr_in& from) {
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
            const float playerWidth = 30.f;
            const float playerHeight = 30.f;
            const float halfSize = 15.f;
            float newX = pos.first + inputX * speed;
            float newY = pos.second + inputY * speed;
            if (!is_position_blocked(newX, pos.second, playerWidth, playerHeight, obstacles))
                pos.first = newX;
            if (!is_position_blocked(pos.first, newY, playerWidth, playerHeight, obstacles))
                pos.second = newY;
            const float maxX = 800.f - halfSize;
            const float maxY = 600.f - halfSize;
            pos.first = std::clamp(pos.first, halfSize, maxX);
            pos.second = std::clamp(pos.second, halfSize, maxY);
        }
    }
}

void ServerGame::broadcast_states_to_clients() {
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

void ServerGame::initialize_obstacles() {
    struct O { float x,y,w,h; }; O list[3] = {
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
    socket.broadcast(&m, sizeof(m));
}

void ServerGame::broadcast_obstacle_despawn(uint32_t obstacleId) {
    ObstacleDespawnMessage m;
    m.type = MessageType::ObstacleDespawn;
    m.obstacleId = htonl(obstacleId);
    socket.broadcast(&m, sizeof(m));
}

void ServerGame::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms) {
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
    }
}