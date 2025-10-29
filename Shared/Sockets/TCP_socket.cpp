/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TCP_socket (Unified Client/Server)
*/

#include "Include/TCP_socket.hpp"
#include "Logger.hpp"

using asio::ip::tcp;

// Client mode constructor
TCP_socket::TCP_socket() 
    : socket(ioContext), 
      acceptor(nullptr), 
      isServerMode(false) {
}

// Server mode constructor
TCP_socket::TCP_socket(uint16_t port)
    : socket(ioContext),
      acceptor(std::make_unique<tcp::acceptor>(ioContext, tcp::endpoint(tcp::v4(), port))),
      isServerMode(true) {
    LOG_INFO(std::format("TCP Server listening on port {}", port));
}

TCP_socket::~TCP_socket() {
    disconnect();
}

bool TCP_socket::connectToServer(const std::string& host, uint16_t port) {
    if (isServerMode) {
        LOG_WARN("connectToServer() called on server-mode socket, ignoring.");
        return false;
    }

    try {
        tcp::resolver resolver(ioContext);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        asio::connect(socket, endpoints);
        LOG_INFO(std::format("Connected to {}:{}",host, port));
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("connectToServer(): {}", e.what()));
        return false;
    }
}

bool TCP_socket::acceptClient() {
    if (!isServerMode) {
        LOG_WARN("acceptClient() called on client-mode socket, ignoring.");
        return false;
    }

    try {
        LOG_INFO("Waiting for client...");
        acceptor->accept(socket);
        LOG_INFO("Client connected.");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("acceptClient(): {}", e.what()));
        return false;
    }
}

void TCP_socket::disconnect() {
    if (socket.is_open()) {
        asio::error_code ec;
        socket.close(ec);
        
        if (isServerMode) {
            LOG_INFO("Client disconnected.");
        } else {
            LOG_INFO("Disconnected from server.");
        }
    }
}

void TCP_socket::sendJson(const nlohmann::json& j) {
    try {
        std::string data = j.dump() + "\n";
        asio::write(socket, asio::buffer(data));
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("sendJson(): {}", e.what()));
    }
}

nlohmann::json TCP_socket::receiveJson() {
    try {
        asio::streambuf buffer;
        asio::read_until(socket, buffer, "\n");
        std::istream is(&buffer);
        std::string line;
        std::getline(is, line);
        return nlohmann::json::parse(line);
    } catch (const std::exception& e) {
        LOG_ERROR(std::format("receiveJson(): {}", e.what()));
        return {};
    }
}

bool TCP_socket::isConnected() const {
    return socket.is_open();
}