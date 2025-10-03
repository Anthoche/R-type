/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#include "Include/connexion.hpp"
#if defined(_WIN32) || defined(_WIN64)
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
#endif

Connexion::Connexion(asio::io_context& io, uint16_t port) : socket(port) {}

void Connexion::asyncReceive(std::function<void(const asio::error_code&, std::vector<uint8_t>, asio::ip::udp::endpoint)> handler) {
    try {
        std::vector<uint8_t> data;
        sockaddr_in clientAddr;
        if (socket.try_receive(data, clientAddr)) {
            asio::ip::udp::endpoint endpoint = UDP_socket::sockaddrToEndpoint(clientAddr);
            asio::error_code ec; 
            handler(ec, std::move(data), endpoint);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] asyncReceive(): " << e.what() << std::endl;
    }
}

void Connexion::sendTo(const void* msg, size_t size, const asio::ip::udp::endpoint& to) {
    try {
        sockaddr_in clientAddr = UDP_socket::endpointToSockaddr(to);
        socket.sendTo(msg, size, clientAddr);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Connexion::sendTo(): " << e.what() << std::endl;
    }
}

void Connexion::broadcast(const void* msg, size_t size) {
    try {
        socket.broadcast(msg, size);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Connexion::broadcast(): " << e.what() << std::endl;
    }
}

void Connexion::addClient(const asio::ip::udp::endpoint& endpoint, uint32_t id) {
    try {
        std::string addrStr = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
        clients[addrStr] = id;
        endpoints[addrStr] = endpoint;

        sockaddr_in clientAddr = UDP_socket::endpointToSockaddr(endpoint);
        socket.addClient(clientAddr, id);

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Connexion::addClient(): " << e.what() << std::endl;
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
                std::cout << "[INFO] Disconnected client: " << it->first << " (ID " << id << ")" << std::endl;
                it = clients.erase(it);
            } else {
                ++it;
            }
        }
        socket.disconnectClient(id);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Connexion::disconnectClient(): " << e.what() << std::endl;
    }
}

bool Connexion::acceptTcpClient(uint32_t id, uint16_t port) {
    try {
        auto client = std::make_shared<TCP_socketServer>(port);
        if (!client->acceptClient())
            return false;
        tcpClients[id] = client;
        std::cout << "[INFO] TCP client " << id << " accepted on port " << port << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] acceptTcpClient(): " << e.what() << std::endl;
        return false;
    }
}

void Connexion::sendJsonToClient(uint32_t id, const nlohmann::json& j) {
    try {
        if (tcpClients.find(id) != tcpClients.end()) {
            tcpClients[id]->sendJson(j);
        } else {
            std::cerr << "[WARN] sendJsonToClient(): client " << id << " not found" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] sendJsonToClient(): " << e.what() << std::endl;
    }
}

void Connexion::broadcastJson(const nlohmann::json& j) {
    try {
        for (auto& [id, client] : tcpClients) {
            client->sendJson(j);
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] broadcastJson(): " << e.what() << std::endl;
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