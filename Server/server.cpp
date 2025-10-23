/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/
#include "Include/server.hpp"
#include "Include/IServerGame.hpp"
#include "../Game/Game_logic/Rtype_game.hpp"
#include <chrono>
#include <thread>
#include <cstring>

GameServer::GameServer(uint16_t port) : connexion(ioContext, port) {}

void GameServer::run()
{
    std::cout << "Serveur démarré. En attente de 4 clients..." << std::endl;
    while (!gameStarted) {
        std::vector<uint8_t> data;
        asio::ip::udp::endpoint clientEndpoint;
        
        bool received = false;
        connexion.asyncReceive([&](const asio::error_code& error, std::vector<uint8_t> receivedData, asio::ip::udp::endpoint sender) {
            if (!error) {
                data = std::move(receivedData);
                clientEndpoint = sender;
                received = true;
            }
        });
        
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        if (!received) {
            continue;
        }
        if (data.size() < sizeof(MessageType))
            continue;
        MessageType type = *reinterpret_cast<MessageType*>(data.data());
        if (type == MessageType::ClientHello) {
            handleClientHello(data, clientEndpoint);
        }
    }
    std::cout << "Tous les clients sont connectés. Le jeu commence !" << std::endl;

    std::string difficulty = "Hard";
    ServerGame game(connexion, difficulty);
    
    game.run();
}

void GameServer::handleClientHello(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& clientEndpoint)
{
    if (gameStarted) return;
    if (data.size() < sizeof(ClientHelloMessage)) return;
    const ClientHelloMessage* msg = reinterpret_cast<const ClientHelloMessage*>(data.data());
    
    uint32_t clientId = nextClientId++;
    connexion.addClient(clientEndpoint, clientId);
    uint16_t tcpPort = 5000 + clientId;
    std::thread([this, clientId, tcpPort]() {
        if (!connexion.acceptTcpClient(clientId, tcpPort)) {
            std::cerr << "[ERROR] TCP accept failed for client " << clientId << std::endl;
        } else {
            std::cout << "[INFO] TCP connection established for client " << clientId << std::endl;
        }
    }).detach();
    ServerAssignIdMessage assignMsg{};
    assignMsg.type = MessageType::ServerAssignId;
    assignMsg.clientId = htonl(clientId);
    connexion.sendTo(&assignMsg, sizeof(assignMsg), clientEndpoint);
    std::cout << "Client connecté : " << msg->clientName
              << " (ID: " << clientId << ")"
              << " [" << connexion.getClientCount() << "/4]" << std::endl;
    if (connexion.getClientCount() == 4) {
        gameStarted = true;
        broadcastGameStart();
    }
}


void GameServer::broadcastGameStart()
{
    GameStartMessage msg{};
    msg.type = MessageType::GameStart;
    msg.clientCount = htonl(4);
    connexion.broadcast(&msg, sizeof(msg));
    std::cout << "[DEBUG] Message GameStart envoyé à tous les clients." << std::endl;
}

void GameServer::handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from)
{
    if (data.size() < sizeof(MessageType)) return;
    MessageType type = *reinterpret_cast<const MessageType*>(data.data());
    if (type != MessageType::ClientInput) return;
    if (data.size() < sizeof(ClientInputMessage)) return;
    const ClientInputMessage* msg = reinterpret_cast<const ClientInputMessage*>(data.data());
    uint32_t id = ntohl(msg->clientId);
    InputCode code = static_cast<InputCode>(msg->inputCode);
    bool pressed = msg->isPressed != 0;
    (void)from;
    std::cout << "[ServerBootstrap][Input] received event from client " << id
              << " code=" << inputCodeToString(code)
              << " pressed=" << pressed << std::endl;
}

void GameServer::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms)
{
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
}
