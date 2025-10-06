/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TCP_socketServer
*/

#include "Include/TCP_socketServer.hpp"

using asio::ip::tcp;

TCP_socketServer::TCP_socketServer(uint16_t port)
    : acceptor(ioContext, tcp::endpoint(tcp::v4(), port)),
      socket(ioContext) {}

TCP_socketServer::~TCP_socketServer() {
    disconnectClient();
}

bool TCP_socketServer::acceptClient() {
    try {
        std::cout << "[INFO] Waiting for client..." << std::endl;
        acceptor.accept(socket);
        std::cout << "[INFO] Client connected." << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] acceptClient(): " << e.what() << std::endl;
        return false;
    }
}

void TCP_socketServer::disconnectClient() {
    if (socket.is_open()) {
        asio::error_code ec;
        socket.close(ec);
        std::cout << "[INFO] Client disconnected." << std::endl;
    }
}

void TCP_socketServer::sendJson(const nlohmann::json& j) {
    try {
        std::string data = j.dump() + "\n";
        asio::write(socket, asio::buffer(data));
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] sendJson(): " << e.what() << std::endl;
    }
}

nlohmann::json TCP_socketServer::receiveJson() {
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

