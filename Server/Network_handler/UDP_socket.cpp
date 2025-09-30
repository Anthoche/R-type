/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** UDP_socket
*/


#include "Include/UDP_socket.hpp"
#include <stdexcept>
#include <cstring>
#if defined(_WIN32)
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <arpa/inet.h>
#endif
using asio::ip::udp;

UDP_socket::UDP_socket(uint16_t port): ioContext(), socket(ioContext) {
    udp::endpoint ep(udp::v4(), port);
    asio::error_code ec;
    socket.open(udp::v4(), ec);
    if (ec) throw std::runtime_error("Failed to open UDP socket: " + ec.message());
    socket.bind(ep, ec);
    if (ec) throw std::runtime_error("Failed to bind UDP socket: " + ec.message());
    socket.non_blocking(true, ec);
    if (ec) throw std::runtime_error("Failed to set non-blocking mode: " + ec.message());
}

UDP_socket::~UDP_socket() {
    asio::error_code ec;
    socket.close(ec);
}

bool UDP_socket::try_receive(std::vector<uint8_t>& outData, sockaddr_in& outAddr) {
    std::array<uint8_t, 2048> buf{};
    udp::endpoint sender;
    asio::error_code ec;

    std::size_t len = socket.receive_from(asio::buffer(buf), sender, 0, ec);

    if (ec) {
        if (ec == asio::error::would_block || ec == asio::error::try_again)
            return false;
        return false;
    }

    outData.assign(buf.data(), buf.data() + len);
    outAddr = endpointToSockaddr(sender);
    return true;
}

void UDP_socket::sendTo(const void* data, size_t size, const sockaddr_in& clientAddr) {
    udp::endpoint ep = sockaddrToEndpoint(clientAddr);
    asio::error_code ec;
    socket.send_to(asio::buffer(data, size), ep, 0, ec);
}

void UDP_socket::broadcast(const void* data, size_t size) {
    std::lock_guard<std::mutex> lock(clientsMutex);
    asio::error_code ec;
    for (const auto& [addrStr, clientId] : clients) {
        auto pos = addrStr.find(':');
        if (pos == std::string::npos) continue;
        std::string ip = addrStr.substr(0, pos);
        uint16_t port = static_cast<uint16_t>(std::stoi(addrStr.substr(pos + 1)));
        auto addr = asio::ip::make_address(ip, ec);
        if (ec) continue;
        udp::endpoint ep(addr, port);
        socket.send_to(asio::buffer(data, size), ep, 0, ec);
    }
}

void UDP_socket::addClient(const sockaddr_in& clientAddr, uint32_t clientId) {
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
    std::string addrStr = std::string(ipStr) + ":" + std::to_string(ntohs(clientAddr.sin_port));
    std::lock_guard<std::mutex> lock(clientsMutex);
    clients[addrStr] = clientId;
}

size_t UDP_socket::getClientCount() const {
    std::lock_guard<std::mutex> lock(clientsMutex);
    return clients.size();
}

std::string UDP_socket::endpointToKey(const asio::ip::udp::endpoint& ep) {
    return ep.address().to_string() + ":" + std::to_string(ep.port());
}

udp::endpoint UDP_socket::sockaddrToEndpoint(const sockaddr_in& in) {
    asio::ip::address_v4 addr(ntohl(in.sin_addr.s_addr));
    return udp::endpoint(addr, ntohs(in.sin_port));
}

sockaddr_in UDP_socket::endpointToSockaddr(const udp::endpoint& ep) {
    sockaddr_in out{};
    out.sin_family = AF_INET;
    out.sin_port = htons(ep.port());
    out.sin_addr.s_addr = htonl(ep.address().to_v4().to_uint());
    return out;
}
