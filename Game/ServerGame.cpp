/*
** EPITECH PROJECT, 2025
** R-Type
*/

#include "ServerGame.hpp"
#include <algorithm>
#include <cstring>
#include <thread>

ServerGame::ServerGame(UDP_socket &sock) : socket(sock) {}

void ServerGame::run() {
    initialize_player_positions();
    initialize_obstacles();
    const int tick_ms = 16;
    while (true) {
        auto tick_start = std::chrono::high_resolution_clock::now();
        process_pending_messages();
        update_enemies(1.f / 60.f);
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

void ServerGame::update_enemies(float dt) {
    enemySpawnTimerSec += dt;
    if (enemySpawnTimerSec >= 2.f) {
        enemySpawnTimerSec = 0.f;
        spawn_enemy();
    }
    std::vector<uint32_t> toDespawn;
    for (auto &kv : enemies) {
        kv.second.first -= 100.f * dt;
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

void ServerGame::spawn_enemy() {
    uint32_t id = nextEnemyId++;
    float x = 800.f - 50.f;
    float y = 100.f + 100.f * ((id - 1) % 4);
    enemies[id] = {x, y};
    broadcast_enemy_spawn(id, x, y);
}

void ServerGame::broadcast_enemy_spawn(uint32_t enemyId, float x, float y) {
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

void ServerGame::broadcast_enemy_update(uint32_t enemyId, float x, float y) {
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

void ServerGame::broadcast_enemy_despawn(uint32_t enemyId) {
    EnemyDespawnMessage m;
    m.type = MessageType::EnemyDespawn;
    m.enemyId = htonl(enemyId);
    socket.broadcast(&m, sizeof(m));
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


