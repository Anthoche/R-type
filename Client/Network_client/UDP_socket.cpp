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

bool UDP_socket::try_receive(std::vector<uint8_t>& outData, sockaddr_in& outAddr) {
    std::array<uint8_t, 2048> buf{};
    udp::endpoint sender;
    asio::error_code ec;

    try {
        std::size_t len = socket.receive_from(asio::buffer(buf), sender, 0, ec);

        if (ec) {
            if (ec == asio::error::would_block || ec == asio::error::try_again)
                return false; // pas de données
            throw std::runtime_error("UDP try_receive failed: " + ec.message());
        }

        outData.assign(buf.data(), buf.data() + len);
        outAddr = endpointToSockaddr(sender);
        return true;

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] try_receive(): " << e.what() << std::endl;
        return false;
    }
}

std::pair<std::vector<uint8_t>, sockaddr_in> UDP_socket::receive() {
    std::array<uint8_t, 2048> buf{};
    udp::endpoint sender;
    asio::error_code ec;

    try {
        std::size_t len = socket.receive_from(asio::buffer(buf), sender, 0, ec);
        if (ec) throw std::runtime_error("UDP receive failed: " + ec.message());

        sockaddr_in addr = endpointToSockaddr(sender);
        return { std::vector<uint8_t>(buf.data(), buf.data() + len), addr };

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] receive(): " << e.what() << std::endl;
        return {};
    }
}

void UDP_socket::sendTo(const void* data, size_t size, const sockaddr_in& clientAddr) {
    try {
        udp::endpoint ep = sockaddrToEndpoint(clientAddr);
        asio::error_code ec;
        socket.send_to(asio::buffer(data, size), ep, 0, ec);
        if (ec) throw std::runtime_error("UDP sendTo failed: " + ec.message());

    } catch (const std::exception& e) {
        std::cerr << "[ERROR] sendTo(): " << e.what() << std::endl;
    }
}

asio::ip::udp::endpoint UDP_socket::sockaddrToEndpoint(const sockaddr_in& in) {
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
