/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#include "Include/connexion.hpp"
#include "Include/UDP_socket.hpp"
#include <arpa/inet.h>

Connexion::Connexion(asio::io_context& io, uint16_t port) : socket(port) {}

void Connexion::asyncReceive(std::function<void(const asio::error_code&, std::vector<uint8_t>, asio::ip::udp::endpoint)> handler) {
    std::vector<uint8_t> data;
    sockaddr_in clientAddr;
    if (socket.try_receive(data, clientAddr)) {
        asio::ip::udp::endpoint endpoint = UDP_socket::sockaddrToEndpoint(clientAddr);
        asio::error_code ec; // Pas d'erreur si on a reçu des données
        handler(ec, std::move(data), endpoint);
    }
}

void Connexion::sendTo(const void* msg, size_t size, const asio::ip::udp::endpoint& to) {
    sockaddr_in clientAddr = UDP_socket::endpointToSockaddr(to);
    socket.sendTo(msg, size, clientAddr);
}

void Connexion::broadcast(const void* msg, size_t size) {
    for (const auto& [addrStr, endpoint] : endpoints) {
        sockaddr_in clientAddr = UDP_socket::endpointToSockaddr(endpoint);
        socket.sendTo(msg, size, clientAddr);
    }
}

void Connexion::addClient(const asio::ip::udp::endpoint& endpoint, uint32_t id) {
    std::string addrStr = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());
    clients[addrStr] = id;
    endpoints[addrStr] = endpoint;
    sockaddr_in clientAddr = UDP_socket::endpointToSockaddr(endpoint);
    socket.addClient(clientAddr, id);
}

size_t Connexion::getClientCount() const {
    return clients.size();
}

const std::unordered_map<std::string, uint32_t>& Connexion::getClients() const {
    retur
const std::unordered_map<std::string, asio::ip::udp::endpoint>& Connexion::getEndpoints() const {
    return endpoints;
}
