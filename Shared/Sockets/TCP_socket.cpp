/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TCP_socket (Unified Client/Server)
*/

#include "Include/TCP_socket.hpp"

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
    std::cout << "[INFO] TCP Server listening on port " << port << std::endl;
}

TCP_socket::~TCP_socket() {
    disconnect();
}

bool TCP_socket::connectToServer(const std::string& host, uint16_t port) {
    if (isServerMode) {
        std::cerr << "[WARN] connectToServer() called on server-mode socket, ignoring." << std::endl;
        return false;
    }

    try {
        tcp::resolver resolver(ioContext);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        asio::connect(socket, endpoints);
        std::cout << "[INFO] Connected to " << host << ":" << port << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] connectToServer(): " << e.what() << std::endl;
        return false;
    }
}

bool TCP_socket::acceptClient() {
    if (!isServerMode) {
        std::cerr << "[WARN] acceptClient() called on client-mode socket, ignoring." << std::endl;
        return false;
    }

    try {
        std::cout << "[INFO] Waiting for client..." << std::endl;
        acceptor->accept(socket);
        std::cout << "[INFO] Client connected." << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] acceptClient(): " << e.what() << std::endl;
        return false;
    }
}

void TCP_socket::disconnect() {
    if (socket.is_open()) {
        asio::error_code ec;
        socket.close(ec);
        
        if (isServerMode) {
            std::cout << "[INFO] Client disconnected." << std::endl;
        } else {
            std::cout << "[INFO] Disconnected from server." << std::endl;
        }
    }
}

void TCP_socket::sendJson(const nlohmann::json& j) {
    try {
        std::string data = j.dump() + "\n";
        asio::write(socket, asio::buffer(data));
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] sendJson(): " << e.what() << std::endl;
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
        std::cerr << "[ERROR] receiveJson(): " << e.what() << std::endl;
        return {};
    }
}

bool TCP_socket::isConnected() const {
    return socket.is_open();
}