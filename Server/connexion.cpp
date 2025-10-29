/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#include "Include/connexion.hpp"
#include "Logger.hpp"
#include <asio.hpp>
#include <chrono>
#include <format>
#include <thread>

Connexion::Connexion(asio::io_context& io, uint16_t port) : socket(port) {}

Connexion::~Connexion() {
    stopListening();
}

void Connexion::startListening() {
    bool expected = false;
    if (!listening.compare_exchange_strong(expected, true))
        return;
    receiverThread = std::thread(&Connexion::receiverLoop, this);
}

void Connexion::stopListening() {
    bool wasListening = listening.exchange(false);
    if (wasListening) {
        queueCv.notify_all();
        if (receiverThread.joinable())
            receiverThread.join();
    }
}

bool Connexion::waitForPacket(ReceivedPacket &packet, std::chrono::milliseconds timeout) {
    std::unique_lock<std::mutex> lock(queueMutex);
    if (!queueCv.wait_for(lock, timeout, [this]() {
        return !packetQueue.empty() || !listening.load();
    }))
        return false;
    if (packetQueue.empty())
        return false;
    packet = std::move(packetQueue.front());
    packetQueue.pop();
    return true;
}

bool Connexion::tryPopPacket(ReceivedPacket &packet) {
    std::lock_guard<std::mutex> lock(queueMutex);
    if (packetQueue.empty())
        return false;
    packet = std::move(packetQueue.front());
    packetQueue.pop();
    return true;
}

void Connexion::broadcastToClients(const std::vector<uint32_t> &clientIds, const void* data, size_t size) {
    if (clientIds.empty())
        return;
    try {
        socket.broadcastToClients(clientIds, data, size);
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Connexion::broadcastToClients(): {}", e.what()));
    }
}

void Connexion::sendTo(const void* msg, size_t size, const asio::ip::udp::endpoint& to) {
    try {
        socket.sendTo(msg, size, to);
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Connexion::sendTo(): {}", e.what()));
    }
}

void Connexion::broadcast(const void* msg, size_t size) {
    try {
        socket.broadcast(msg, size);
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Connexion::broadcast(): {}", e.what()));
    }
}

void Connexion::broadcastToRoom(Room const &room, const void* msg, size_t size) {
    try {
        socket.broadcastToClients(room.getClients(), msg, size);
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Connexion::broadcastToRoom(): {}", e.what()));
    }
}

void Connexion::addClient(const asio::ip::udp::endpoint& endpoint, uint32_t id) {
    try {
        std::string addrStr = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        clients[addrStr] = id;
        endpoints[addrStr] = endpoint;

        socket.addClient(endpoint, id);

    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Connexion::addClient(): {}", e.what()));
    }
}

void Connexion::disconnectClient(uint32_t id) {
    try {
        // Nettoyer la map endpoints
        for (auto it = endpoints.begin(); it != endpoints.end();) {
            if (clients[it->first] == id) {
                it = endpoints.erase(it);
            } else {
                ++it;
            }
        }
        for (auto it = clients.begin(); it != clients.end();) {
            if (it->second == id) {
                LOG_INFO(std::format("Disconnected client: {}({})", it->first, id));
                it = clients.erase(it);
            } else {
                ++it;
            }
        }
        clientNames.erase(id);
        socket.disconnectClient(id);
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Connexion::disconnectClient(): {}", e.what()));
    }
}

bool Connexion::acceptTcpClient(uint32_t id, uint16_t port) {
    try {
        auto client = std::make_shared<TCP_socket>(port);
        if (!client->acceptClient())
            return false;
        tcpClients[id] = client;
        LOG_INFO(std::format("TCP client {} accepted on port {}", id, port));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("acceptTcpClient(): {}", e.what()));
        return false;
    }
}

void Connexion::sendJsonToClient(uint32_t id, const nlohmann::json& j) {
    try {
        if (tcpClients.find(id) != tcpClients.end()) {
            tcpClients[id]->sendJson(j);
        } else {
            LOG_WARN(std::format("sendJsonToClient(): client {} not found", id));
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("sendJsonToClient(): ", e.what()));
    }
}

void Connexion::broadcastJson(const nlohmann::json& j) {
    try {
        for (auto& [id, client] : tcpClients) {
            client->sendJson(j);
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("broadcastJson(): {}", e.what()));
    }
}

void Connexion::setClientName(uint32_t id, std::string name) {
    clientNames[id] = std::move(name);
}

std::string Connexion::getClientName(uint32_t id) const {
    auto it = clientNames.find(id);
    if (it != clientNames.end())
        return it->second;
    return {};
}

size_t Connexion::getClientCount() const {
    return clients.size();
}

const std::unordered_map<std::string, uint32_t>& Connexion::getClients() const {
    return clients;
}

const std::unordered_map<std::string, asio::ip::udp::endpoint>& Connexion::getEndpoints() const {
    return endpoints;
}

void Connexion::receiverLoop() {
    while (listening.load()) {
        try {
            std::vector<uint8_t> data;
            asio::ip::udp::endpoint endpoint;
            if (socket.try_receive(data, endpoint) && !data.empty()) {
                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    packetQueue.push(ReceivedPacket{std::move(data), endpoint});
                }
                queueCv.notify_one();
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        } catch (const std::exception& e) {
            LOG_ERROR(std::format("Connexion::receiverLoop(): {}", e.what()));
        }
    }
}