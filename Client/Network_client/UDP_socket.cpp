/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** udp_socket
*/

#include "Include/UDP_socket.hpp"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <asio.hpp>

using asio::ip::udp;

UDP_socket::UDP_socket() : socket(ioContext) {
    try {
        asio::error_code ec;
        socket.open(udp::v4(), ec);
        if (ec) throw std::runtime_error("Failed to open UDP socket: " + ec.message());

        socket.non_blocking(true, ec);
        if (ec) throw std::runtime_error("Failed to set non-blocking mode: " + ec.message());

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] UDP_socket ctor: " << e.what() << std::endl;
        throw; // rethrow pour ne pas masquer une erreur critique
    }
}

UDP_socket::~UDP_socket() {
    asio::error_code ec;
    socket.close(ec);
}

bool UDP_socket::try_receive(std::vector<uint8_t>& outData, asio::ip::udp::endpoint& outEndpoint) {
    std::array<uint8_t, 2048> buf{};
    asio::error_code ec;

    try {
        std::size_t len = socket.receive_from(asio::buffer(buf), outEndpoint, 0, ec);

        if (ec) {
            if (ec == asio::error::would_block || ec == asio::error::try_again)
                return false; // pas de données
            throw std::runtime_error("UDP try_receive failed: " + ec.message());
        }

        outData.assign(buf.data(), buf.data() + len);
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] try_receive(): " << e.what() << std::endl;
        return false;
    }
}

std::pair<std::vector<uint8_t>, asio::ip::udp::endpoint> UDP_socket::receive() {
    std::array<uint8_t, 2048> buf{};
    udp::endpoint sender;
    asio::error_code ec;

    try {
        std::size_t len = socket.receive_from(asio::buffer(buf), sender, 0, ec);
        if (ec) throw std::runtime_error("UDP receive failed: " + ec.message());

        return { std::vector<uint8_t>(buf.data(), buf.data() + len), sender };

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] receive(): " << e.what() << std::endl;
        return {};
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