/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client
*/

#include "Include/client.hpp"
#include <functional>
#include <chrono>
#include <SFML/Window/Keyboard.hpp>


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
    uint32_t count = waitForGameStart();
    runGameLoop(count);
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
    uint32_t colorIdx;

    if (clientId > 0) {
        colorIdx = (clientId - 1) % 4;
    } else {
        std::size_t nameHash = std::hash<std::string>{}(clientName);
        colorIdx = static_cast<uint32_t>(nameHash % 4);
    }
    applyPlayerColorByIndex(scene, colorIdx);

    auto last = std::chrono::high_resolution_clock::now();
    while (scene.window_is_open()) {
        auto now = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float>(now - last).count();
        last = now;
        gameFrame(scene, dt);
    }
}

void GameClient::applyPlayerColorByIndex(game::scene::GameScene &scene, uint32_t idx) {
    std::cout << idx << std::endl;
    const float colors[4][3] = {
        {0.f, 1.f, 1.f}, // cyan
        {1.f, 0.f, 0.f}, // rouge
        {0.f, 1.f, 0.f}, // vert
        {1.f, 1.f, 0.f}  // jaune
    };
    idx %= 4;
    scene.set_local_player_color(colors[idx][0], colors[idx][1], colors[idx][2], 1.f);
}

uint32_t GameClient::waitForGameStart() {
    std::cout << "Client démarré. En attente du début du jeu..." << std::endl;
    while (true) {
        std::vector<uint8_t> buffer(1024);
        sockaddr_in fromAddr = {};
        socklen_t fromAddrLen = sizeof(fromAddr);
        ssize_t bytesReceived = recvfrom(socketFd, buffer.data(), buffer.size(), 0, (struct sockaddr*)&fromAddr, &fromAddrLen);
        if (bytesReceived < static_cast<ssize_t>(sizeof(MessageType))) continue;
        MessageType* type = reinterpret_cast<MessageType*>(buffer.data());
        if (*type == MessageType::ServerAssignId && bytesReceived >= static_cast<ssize_t>(sizeof(ServerAssignIdMessage))) {
            const ServerAssignIdMessage* msg = reinterpret_cast<const ServerAssignIdMessage*>(buffer.data());
            clientId = ntohl(msg->clientId);
            std::cout << "[INFO] ID serveur reçu: " << clientId << std::endl;
        } else if (*type == MessageType::GameStart && bytesReceived >= static_cast<ssize_t>(sizeof(GameStartMessage))) {
            const GameStartMessage* msg = reinterpret_cast<const GameStartMessage*>(buffer.data());
            uint32_t count = ntohl(msg->clientCount);
            std::cout << "Le jeu commence ! Nombre de joueurs : " << count << std::endl;
            return count;
        }
    }
}

void GameClient::gatherInput(float &inputX, float &inputY) const {
    inputX = 0.f; inputY = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) inputX -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) inputX += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) inputY -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) inputY += 1.f;
}

void GameClient::sendInputToServer(float inputX, float inputY) const {
    ClientInputMessage imsg;
    imsg.type = MessageType::ClientInput;
    imsg.clientId = htonl(clientId);
    uint32_t xbits, ybits;
    std::memcpy(&xbits, &inputX, sizeof(float));
    std::memcpy(&ybits, &inputY, sizeof(float));
    imsg.inputXBits = htonl(xbits);
    imsg.inputYBits = htonl(ybits);
    sendto(socketFd, &imsg, sizeof(imsg), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
}

void GameClient::readStateUpdates(game::scene::GameScene &scene) const {
    while (true) {
        std::vector<uint8_t> buffer(1024);
        sockaddr_in fromAddr = {};
        socklen_t fromLen = sizeof(fromAddr);
        ssize_t bytes = recvfrom(socketFd, buffer.data(), buffer.size(), MSG_DONTWAIT, (struct sockaddr*)&fromAddr, &fromLen);
        if (bytes <= 0) break;
        if (static_cast<size_t>(bytes) < sizeof(MessageType)) continue;
        MessageType* t = reinterpret_cast<MessageType*>(buffer.data());
        if (*t == MessageType::StateUpdate && static_cast<size_t>(bytes) >= sizeof(StateUpdateMessage)) {
            const StateUpdateMessage* smsg = reinterpret_cast<const StateUpdateMessage*>(buffer.data());
            uint32_t id = ntohl(smsg->clientId);
            uint32_t xbitsN = ntohl(smsg->posXBits);
            uint32_t ybitsN = ntohl(smsg->posYBits);
            float px, py;
            std::memcpy(&px, &xbitsN, sizeof(float));
            std::memcpy(&py, &ybitsN, sizeof(float));
            if (id == clientId) {
                scene.set_local_player_position(px, py);
            } else {
                scene.ensure_remote_player(id);
                // couleur déterministe par id
                const float colors[4][3] = {
                    {0.f, 1.f, 1.f},
                    {1.f, 0.f, 0.f},
                    {0.f, 1.f, 0.f},
                    {1.f, 1.f, 0.f}
                };
                uint32_t idx = (id - 1) % 4;
                scene.set_remote_player_color(id, colors[idx][0], colors[idx][1], colors[idx][2], 1.f);
                scene.set_remote_player_position(id, px, py);
            }
        } else if (*t == MessageType::EnemySpawn && static_cast<size_t>(bytes) >= sizeof(EnemySpawnMessage)) {
            const EnemySpawnMessage* em = reinterpret_cast<const EnemySpawnMessage*>(buffer.data());
            uint32_t enemyId = ntohl(em->enemyId);
            uint32_t xbitsN = ntohl(em->posXBits);
            uint32_t ybitsN = ntohl(em->posYBits);
            float ex, ey;
            std::memcpy(&ex, &xbitsN, sizeof(float));
            std::memcpy(&ey, &ybitsN, sizeof(float));
            scene.ensure_enemy(enemyId, ex, ey);
        } else if (*t == MessageType::EnemyUpdate && static_cast<size_t>(bytes) >= sizeof(EnemyUpdateMessage)) {
            const EnemyUpdateMessage* em = reinterpret_cast<const EnemyUpdateMessage*>(buffer.data());
            uint32_t enemyId = ntohl(em->enemyId);
            uint32_t xbitsN = ntohl(em->posXBits);
            uint32_t ybitsN = ntohl(em->posYBits);
            float ex, ey;
            std::memcpy(&ex, &xbitsN, sizeof(float));
            std::memcpy(&ey, &ybitsN, sizeof(float));
            scene.set_enemy_position(enemyId, ex, ey);
        } else if (*t == MessageType::EnemyDespawn && static_cast<size_t>(bytes) >= sizeof(EnemyDespawnMessage)) {
            const EnemyDespawnMessage* em = reinterpret_cast<const EnemyDespawnMessage*>(buffer.data());
            uint32_t enemyId = ntohl(em->enemyId);
            scene.despawn_enemy(enemyId);
        }
    }
}

void GameClient::gameFrame(game::scene::GameScene &scene, float dt) {
    scene.poll_events();
    float inputX = 0.f, inputY = 0.f;
    if (scene.window_has_focus()) {
        gatherInput(inputX, inputY);
        scene.handle_input(inputX, inputY);
        sendInputToServer(inputX, inputY);
    } else { 
        // Pas de focus: ne pas envoyer d'input, et forcer vitesse nulle côté local
        scene.handle_input(0.f, 0.f);
    }
    readStateUpdates(scene);
    scene.update(dt);
    scene.render();
}

