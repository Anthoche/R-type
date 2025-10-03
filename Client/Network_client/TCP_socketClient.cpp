/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TCP_socket
*/

#include "Include/TCP_socketClient.hpp"

using asio::ip::tcp;

TCP_socketClient::TCP_socketClient() : socket(ioContext) {}

TCP_socketClient::~TCP_socketClient() {
    disconnect();
}

bool TCP_socketClient::connectToServer(const std::string& host, uint16_t port) {
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

void TCP_socketClient::disconnect() {
    if (socket.is_open()) {
        asio::error_code ec;
        socket.close(ec);
    }
}

void TCP_socketClient::sendJson(const nlohmann::json& j) {
    try {
        std::string data = j.dump() + "\n"; // délimiteur
        asio::write(socket, asio::buffer(data));
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] sendJson(): " << e.what() << std::endl;
    }
}

nlohmann::json TCP_socketClient::receiveJson() {
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
