/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** client_handler
*/

#include "../client.hpp"
#include "../../Engine/Game.hpp"
#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
#endif


void GameClient::handleMessage(MessageType type, const std::vector<uint8_t> &buffer) {
    switch (type) {
        case MessageType::ServerAssignId:
            handleServerAssignId(buffer);
            break;
        case MessageType::GameStart:
            handleGameStart(buffer);
            break;
        case MessageType::StateUpdate:
            handlePlayerUpdate(buffer);
            break;
        case MessageType::ObstacleSpawn:
            handleObstacleSpawn(buffer);
            break;
        case MessageType::ObstacleDespawn:
            handleObstacleDespawn(buffer);
            break;
        default:
            break;
    }
}


void GameClient::handleServerAssignId(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ServerAssignIdMessage)) return;
    const ServerAssignIdMessage *msg = reinterpret_cast<const ServerAssignIdMessage *>(buffer.data());
    clientId = ntohl(msg->clientId);
    std::cout << "[Client] Reçu clientId=" << clientId << std::endl;
    initTcpConnection();
}

void GameClient::handleGameStart(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(GameStartMessage)) return;
    const GameStartMessage *msg = reinterpret_cast<const GameStartMessage *>(buffer.data());
    std::cout << "Le jeu commence ! Nombre de joueurs : " << ntohl(msg->clientCount) << std::endl;
    _game.setGameStatus(GameStatus::RUNNING);
}

void GameClient::handlePlayerUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(StateUpdateMessage)) return;
    const StateUpdateMessage *msg = reinterpret_cast<const StateUpdateMessage *>(buffer.data());
    uint32_t id = ntohl(msg->clientId);
    uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
    float x, y;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::lock_guard<std::mutex> g(stateMutex);
    players[id] = {x, y};
}

void GameClient::handleObstacleSpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ObstacleSpawnMessage)) return;
    const ObstacleSpawnMessage *msg = reinterpret_cast<const ObstacleSpawnMessage *>(buffer.data());
    uint32_t id = ntohl(msg->obstacleId);
    uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
    uint32_t wb = ntohl(msg->widthBits), hb = ntohl(msg->heightBits);
    float x, y, w, h;
    std::memcpy(&x, &xb, 4);
    std::memcpy(&y, &yb, 4);
    std::memcpy(&w, &wb, 4);
    std::memcpy(&h, &hb, 4);
    std::lock_guard<std::mutex> g(stateMutex);
    obstacles[id] = std::make_tuple(x, y, w, h);
}

void GameClient::handleObstacleDespawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ObstacleDespawnMessage)) return;
    const ObstacleDespawnMessage *msg = reinterpret_cast<const ObstacleDespawnMessage *>(buffer.data());
    uint32_t id = ntohl(msg->obstacleId);
    std::lock_guard<std::mutex> g(stateMutex);
    obstacles.erase(id);
}