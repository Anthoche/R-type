/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#include "Include/client.hpp"


GameClient::GameClient(const std::string& serverIp, uint16_t serverPort, const std::string& name)
    : clientName(name) {
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (socketFd < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    if (inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid server IP");
    }
    sendHello();
}

GameClient::~GameClient() {
    close(socketFd);
}

void GameClient::run() {
    std::cout << "Client démarré. En attente du début du jeu..." << std::endl;
    while (true) {
        std::vector<uint8_t> buffer(1024);
        sockaddr_in fromAddr = {};
        socklen_t fromAddrLen = sizeof(fromAddr);
        ssize_t bytesReceived = recvfrom(
            socketFd, buffer.data(), buffer.size(), 0,
            (struct sockaddr*)&fromAddr, &fromAddrLen
        );

        if (bytesReceived < sizeof(MessageType)) {
            continue;
        }
        MessageType* type = reinterpret_cast<MessageType*>(buffer.data());
        if (*type == MessageType::GameStart) {
            if (bytesReceived >= sizeof(GameStartMessage)) {
                const GameStartMessage* msg = reinterpret_cast<const GameStartMessage*>(buffer.data());
                uint32_t count = ntohl(msg->clientCount);
                std::cout << "Le jeu commence ! Nombre de joueurs : " << count << std::endl;
                runGameLoop(count);
                break;
            }
        }
    }
}

void GameClient::sendHello() {
    ClientHelloMessage msg;
    msg.type = MessageType::ClientHello;
    msg.clientId = htonl(0);
    strncpy(msg.clientName, clientName.c_str(), sizeof(msg.clientName) - 1);
    msg.clientName[sizeof(msg.clientName) - 1] = '\0';
    ssize_t sentBytes = sendto(
        socketFd, &msg, sizeof(msg), 0,
        (struct sockaddr*)&serverAddr, sizeof(serverAddr)
    );

    if (sentBytes < 0) {
        std::cerr << "[ERREUR] Échec de l'envoi de ClientHello" << std::endl;
    }
}

void GameClient::runGameLoop(uint32_t clientCount) {
    (void)clientCount;
    game::scene::GameScene scene(800, 600);

    // En l'absence d'ID serveur, on attribue une couleur par index modulo 4.
    applyPlayerColorByIndex(scene, clientId % 4);

    auto last = std::chrono::high_resolution_clock::now();
    while (scene.window_is_open()) {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;
        scene.poll_events();
        // TODO: intégrer de vrais inputs / messages réseau
        scene.handle_input(0.f, 0.f);
        scene.update(dt);
        scene.render();
    }
}

void GameClient::applyPlayerColorByIndex(game::scene::GameScene &scene, uint32_t idx) {
    const float colors[4][3] = {
        {0.f, 1.f, 1.f}, // cyan
        {1.f, 0.f, 0.f}, // rouge
        {0.f, 1.f, 0.f}, // vert
        {1.f, 1.f, 0.f}  // jaune
    };
    idx %= 4;
    scene.set_local_player_color(colors[idx][0], colors[idx][1], colors[idx][2], 1.f);
}

