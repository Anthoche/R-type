/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** server
*/
#include "server.hpp"
#include "Game_logic/Include/ServerGame.hpp"
#include <chrono>
#include <thread>
#include <cstring>

GameServer::GameServer(uint16_t port) : connexion(ioContext, port) {}

void GameServer::run()
{
    std::cout << "Serveur démarré. En attente de 4 clients..." << std::endl;
    // accept clients until game start
    while (!gameStarted) {
        std::vector<uint8_t> data;
        asio::ip::udp::endpoint clientEndpoint;
        
        // Utilisation d'asyncReceive avec un handler synchrone
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
    // Delegate to Game module
    ServerGame game(connexion);
    game.run();
}

void GameServer::handleClientHello(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& clientEndpoint)
{
    if (gameStarted) return;
    if (data.size() < sizeof(ClientHelloMessage)) return;

    const ClientHelloMessage* msg = reinterpret_cast<const ClientHelloMessage*>(data.data());
    uint32_t clientId = nextClientId++;
    connexion.addClient(clientEndpoint, clientId);
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
    uint32_t xbits = ntohl(msg->inputXBits);
    uint32_t ybits = ntohl(msg->inputYBits);
    float inputX, inputY;
    std::memcpy(&inputX, &xbits, sizeof(float));
    std::memcpy(&inputY, &ybits, sizeof(float));
    auto& pos = playerPositions[id];
    float speed = 200.f / 60.f; // units per tick
    pos.first += inputX * speed;
    pos.second += inputY * speed;
    const float halfSize = 15.f;
    pos.first = std::clamp(pos.first, halfSize, 800.f - halfSize);
    pos.second = std::clamp(pos.second, halfSize, 600.f - halfSize);
}

void GameServer::sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms)
{
    auto tick_end = std::chrono::high_resolution_clock::now();
    auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tick_end - start).count();
    if (elapsed_ms < tick_ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed_ms));
}
