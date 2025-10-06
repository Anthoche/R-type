#pragma once

#include <asio.hpp>
#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>
#include <cstdint>
#include <iostream>
#if defined(_WIN32) || defined(_WIN64)
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <netinet/in.h>
#endif

class UDP_socket {
public:
    explicit UDP_socket(uint16_t port);
    ~UDP_socket();

    // --- réception ---
    std::pair<std::vector<uint8_t>, sockaddr_in> receive(); // blocant
    bool try_receive(std::vector<uint8_t>& outData, sockaddr_in& outAddr); // non bloquant

    // --- envoi ---
    void sendTo(const void* data, size_t size, const sockaddr_in& clientAddr);
    void broadcast(const void* data, size_t size);

    // --- gestion clients ---
    void addClient(const sockaddr_in& clientAddr, uint32_t clientId);
    void disconnectClient(uint32_t clientId); // suppression client
    size_t getClientCount() const;

    const std::unordered_map<std::string, uint32_t>& getClients() const { return clients; }

    // --- utilitaires ---
    static std::string endpointToKey(const asio::ip::udp::endpoint& ep);
    static asio::ip::udp::endpoint sockaddrToEndpoint(const sockaddr_in& in);
    static sockaddr_in endpointToSockaddr(const asio::ip::udp::endpoint& ep);

private:
    asio::io_context ioContext;
    asio::ip::udp::socket socket;
    std::unordered_map<std::string, uint32_t> clients;
    mutable std::mutex clientsMutex;
};
