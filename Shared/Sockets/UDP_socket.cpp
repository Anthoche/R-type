/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** UDP_socket (Unified Client/Server)
*/

#include "Include/UDP_socket.hpp"
#include <stdexcept>
#include <cstring>
#include <asio.hpp>

using asio::ip::udp;

// Client mode constructor (no binding)
UDP_socket::UDP_socket() : socket(ioContext), isServerMode(false) {
    try {
        asio::error_code ec;
        socket.open(udp::v4(), ec);
        if (ec) throw std::runtime_error("Failed to open UDP socket: " + ec.message());

        socket.non_blocking(true, ec);
        if (ec) throw std::runtime_error("Failed to set non-blocking mode: " + ec.message());

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] UDP_socket client ctor: " << e.what() << std::endl;
        throw;
    }
}

// Server mode constructor (with binding)
UDP_socket::UDP_socket(uint16_t port) : ioContext(), socket(ioContext), isServerMode(true) {
    udp::endpoint ep(udp::v4(), port);
    asio::error_code ec;

    try {
        socket.open(udp::v4(), ec);
        if (ec) throw std::runtime_error("Failed to open UDP socket: " + ec.message());

        socket.bind(ep, ec);
        if (ec) throw std::runtime_error("Failed to bind UDP socket: " + ec.message());

        socket.non_blocking(true, ec);
        if (ec) throw std::runtime_error("Failed to set non-blocking mode: " + ec.message());

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] UDP_socket server ctor: " << e.what() << std::endl;
        throw;
    }
}

UDP_socket::~UDP_socket() {
    asio::error_code ec;
    socket.close(ec);
}

std::pair<std::vector<uint8_t>, asio::ip::udp::endpoint> UDP_socket::receive() {
    std::array<uint8_t, 2048> buf{};
    udp::endpoint sender;
    asio::error_code ec;

    try {
        size_t len = socket.receive_from(asio::buffer(buf), sender, 0, ec);
        if (ec) throw std::runtime_error("UDP receive failed: " + ec.message());

        return { std::vector<uint8_t>(buf.data(), buf.data() + len), sender };
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] receive(): " << e.what() << std::endl;
        return {};
    }
}

bool UDP_socket::try_receive(std::vector<uint8_t>& outData, asio::ip::udp::endpoint& outEndpoint) {
    std::array<uint8_t, 2048> buf{};
    asio::error_code ec;

    try {
        std::size_t len = socket.receive_from(asio::buffer(buf), outEndpoint, 0, ec);

        if (ec) {
            if (ec == asio::error::would_block || ec == asio::error::try_again)
                return false;
            throw std::runtime_error("UDP try_receive failed: " + ec.message());
        }

        outData.assign(buf.data(), buf.data() + len);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] try_receive(): " << e.what() << std::endl;
        return false;
    }
}

void UDP_socket::sendTo(const void* data, size_t size, const asio::ip::udp::endpoint& endpoint) {
    try {
        asio::error_code ec;
        socket.send_to(asio::buffer(data, size), endpoint, 0, ec);
        if (ec) throw std::runtime_error("UDP sendTo failed: " + ec.message());
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] sendTo(): " << e.what() << std::endl;
    }
}

void UDP_socket::broadcast(const void* data, size_t size) {
    if (!isServerMode) {
        std::cerr << "[WARN] broadcast() called on client-mode socket, ignoring." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(clientsMutex);
    asio::error_code ec;

    for (const auto& [addrStr, clientId] : clients) {
        try {
            auto pos = addrStr.find(':');
            if (pos == std::string::npos) continue;
            
            std::string ip = addrStr.substr(0, pos);
            uint16_t port = static_cast<uint16_t>(std::stoi(addrStr.substr(pos + 1)));
            auto addr = asio::ip::make_address(ip, ec);
            if (ec) continue;

            udp::endpoint ep(addr, port);
            socket.send_to(asio::buffer(data, size), ep, 0, ec);
            if (ec) throw std::runtime_error("UDP broadcast failed to " + addrStr + ": " + ec.message());

        } catch (const std::exception& e) {
            std::cerr << "[WARN] broadcast(): " << e.what() << std::endl;
        }
    }
}

void UDP_socket::broadcastToClients(std::vector<uint32_t> const &roomClients, const void* data, size_t size) {
    if (!isServerMode) {
        std::cerr << "[WARN] broadcastToRoom() called on client-mode socket, ignoring." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(clientsMutex);
    asio::error_code ec;

    for (auto &p : roomClients) {
        for (const auto& [addrStr, clientId] : clients) {
            if (p != clientId)
                continue;

            try {
                auto pos = addrStr.find(':');
                if (pos == std::string::npos) continue;

                std::string ip = addrStr.substr(0, pos);
                uint16_t port = static_cast<uint16_t>(std::stoi(addrStr.substr(pos + 1)));
                auto addr = asio::ip::make_address(ip, ec);
                if (ec) continue;

                udp::endpoint ep(addr, port);
                socket.send_to(asio::buffer(data, size), ep, 0, ec);
                if (ec) throw std::runtime_error("UDP broadcast failed to " + addrStr + ": " + ec.message());

            } catch (const std::exception& e) {
                std::cerr << "[WARN] broadcast(): " << e.what() << std::endl;
            }
        }
    }
}

void UDP_socket::addClient(const asio::ip::udp::endpoint& endpoint, uint32_t clientId) {
    if (!isServerMode) {
        std::cerr << "[WARN] addClient() called on client-mode socket, ignoring." << std::endl;
        return;
    }

    std::string addrStr = endpoint.address().to_string() + ":" + std::to_string(endpoint.port());

    std::lock_guard<std::mutex> lock(clientsMutex);
    clients[addrStr] = clientId;
}

void UDP_socket::disconnectClient(uint32_t clientId) {
    if (!isServerMode) {
        std::cerr << "[WARN] disconnectClient() called on client-mode socket, ignoring." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(clientsMutex);

    for (auto it = clients.begin(); it != clients.end();) {
        if (it->second == clientId) {
            std::cout << "[INFO] Client disconnected: " << it->first << " (ID " << clientId << ")" << std::endl;
            it = clients.erase(it);
        } else {
            ++it;
        }
    }
}

size_t UDP_socket::getClientCount() const {
    if (!isServerMode) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(clientsMutex);
    return clients.size();
}