/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/
#include "Include/client.hpp"
#include "../../Engine/Game.hpp"
#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
#endif


GameClient::GameClient(Game &game, const std::string &serverIp, uint16_t serverPort, const std::string &name)
    : clientName(name), _game(game) {
    serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid server IP");
    }
}

GameClient::~GameClient() {
    running = false;
    if (rxThread.joinable()) rxThread.join();
}

void GameClient::run() {
    std::cout << "Client démarré. En attente du début du jeu..." << std::endl;
    running = true;
    rxThread = std::thread(&GameClient::recvLoop, this);
    while (_game.getGameStatus() != GameStatus::RUNNING) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "Le jeu commence, ouverture de la fenêtre client..." << std::endl;
}

void GameClient::sendHello() {
    ClientHelloMessage msg;
    msg.type = MessageType::ClientHello;
    msg.clientId = htonl(0);
    strncpy(msg.clientName, clientName.c_str(), sizeof(msg.clientName) - 1);
    msg.clientName[sizeof(msg.clientName) - 1] = '\0';
    socket.sendTo(&msg, sizeof(msg), serverAddr);
}

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

void GameClient::recvLoop() {
    while (running) {
        std::vector<uint8_t> buffer;
        sockaddr_in fromAddr = {};
        if (socket.try_receive(buffer, fromAddr)) {
            if (buffer.size() < sizeof(MessageType)) {
                continue;
            }
            MessageType type = *reinterpret_cast<MessageType *>(buffer.data());
            handleMessage(type, buffer);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    }
}

void GameClient::handleServerAssignId(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ServerAssignIdMessage)) return;
    const ServerAssignIdMessage *msg = reinterpret_cast<const ServerAssignIdMessage *>(buffer.data());
    clientId = ntohl(msg->clientId);
    std::cout << "[Client] Reçu clientId=" << clientId << std::endl;
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

void GameClient::sendInput(float inputX, float inputY) {
    ClientInputMessage m{};
    m.type = MessageType::ClientInput;
    m.clientId = htonl(clientId);
    uint32_t xbits, ybits;
    std::memcpy(&xbits, &inputX, sizeof(float));
    std::memcpy(&ybits, &inputY, sizeof(float));
    m.inputXBits = htonl(xbits);
    m.inputYBits = htonl(ybits);
    socket.sendTo(&m, sizeof(m), serverAddr);
}

void GameClient::runRenderLoop() {
    /* ... (code commenté inchangé) ... */
}
