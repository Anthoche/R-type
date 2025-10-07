/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/
#include "client.hpp"
#include "../Engine/Game.hpp"
#include <asio.hpp>

GameClient::GameClient(Game &game, const std::string &serverIp, uint16_t serverPort, const std::string &name)
    : clientName(name), _game(game), serverIpStr(serverIp) {
    try {
        serverEndpoint = asio::ip::udp::endpoint(
            asio::ip::address::from_string(serverIp), 
            serverPort
        );
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid server IP: " + std::string(e.what()));
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
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);
}

void GameClient::initTcpConnection() {
    if (clientId == 0) return;

    uint16_t tcpPort = 5000 + clientId;
    tcpClient = std::make_unique<TCP_socketClient>();

    if (!tcpClient->connectToServer(serverIpStr, tcpPort)) {
        std::cerr << "[Client] Impossible de se connecter en TCP" << std::endl;
        return;
    }
}

void GameClient::recvLoop() {
    while (running) {
        std::vector<uint8_t> buffer;
        asio::ip::udp::endpoint fromEndpoint;
        if (socket.try_receive(buffer, fromEndpoint)) {
            if (buffer.size() < sizeof(MessageType)) {
                continue;
            }
            MessageType type = *reinterpret_cast<MessageType *>(buffer.data());
            handleMessage(type, buffer);
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
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
    socket.sendTo(&m, sizeof(m), serverEndpoint);
}

void GameClient::sendSceneState(SceneState scene) {
    SceneStateMessage msg{};
    msg.type = MessageType::SceneState;
    msg.clientId = htonl(clientId);
    msg.scene = htonl(static_cast<uint32_t>(scene));
    std::cout << "Le jeu commence, envoie de la scene" << std::endl;
    socket.sendTo(&msg, sizeof(msg), serverEndpoint);

    if (scene == SceneState::GAME) {
        if (!tcpClient) {
            initTcpConnection();
            if (!tcpClient) {
                std::cerr << "[ERROR] Impossible de se connecter au serveur TCP pour récupérer le full registry" << std::endl;
                return;
            }
        }
        std::cout << "[INFO] En attente du full registry du serveur..." << std::endl;
        nlohmann::json fullRegistry = tcpClient->receiveJson();
        if (fullRegistry.is_null()) {
            std::cerr << "[ERROR] Réception du JSON échouée" << std::endl;
            return;
        }
        std::cout << "[DEBUG] JSON reçu du serveur: " << fullRegistry.dump() << std::endl;
    }
}

void GameClient::sendShoot() {
    if (clientId == 0)
        return;
    ClientShootMessage msg;
    msg.type = MessageType::ClientShoot;
    msg.clientId = htonl(clientId);
    socket.sendTo( &msg, sizeof(msg) ,serverEndpoint);
}
