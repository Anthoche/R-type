/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#include "Include/connexion.hpp"
#include "Logger.hpp"
#include <asio.hpp>

Connexion::Connexion(asio::io_context& io, uint16_t port) : socket(port) {}

void Connexion::asyncReceive(std::function<void(const asio::error_code&, std::vector<uint8_t>, asio::ip::udp::endpoint)> handler) {
    try {
        std::vector<uint8_t> data;
        asio::ip::udp::endpoint clientEndpoint;
        if (socket.try_receive(data, clientEndpoint)) {
            asio::error_code ec; 
            handler(ec, std::move(data), clientEndpoint);
        }
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("Connexion::asyncReceive(): {}", e.what()));
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
        socket.broadcastToRoom(room, msg, size);
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

size_t Connexion::getClientCount() const {
    return clients.size();
}

const std::unordered_map<std::string, uint32_t>& Connexion::getClients() const {
    return clients;
}

const std::unordered_map<std::string, asio::ip::udp::endpoint>& Connexion::getEndpoints() const {
    return endpoints;
}