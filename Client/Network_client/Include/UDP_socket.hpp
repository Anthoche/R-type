/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** UDP_socket
*/

#pragma once

#include <asio.hpp>
#include <vector>
#include <string>
#include <cstdint>
#if defined(_WIN32) || defined(_WIN64)
    #include <WinSock2.h>
    #include <ws2tcpip.h>
#else
    #include <netinet/in.h>
#endif

/**
 * @brief UDP socket wrapper for cross-platform client network communication.
 *
 * Manages a UDP socket for client-side network operations.
 * Provides a unified interface for both Windows and Linux.
 */
class UDP_socket {
public:
    /**
     * @brief Constructs a UDP_socket.
     * @throws std::runtime_error if socket creation fails.
     */
    UDP_socket();

    /**
     * @brief Destroys the UDP_socket and closes the socket.
     */
    ~UDP_socket();

    /**
     * @brief Attempts to receive a UDP message (non-blocking).
     * @param outData Output parameter for the received data.
     * @param outAddr Output parameter for the sender's address.
     * @return true if a message was received, false otherwise.
     */
    bool try_receive(std::vector<uint8_t>& outData, sockaddr_in& outAddr);

    /**
     * @brief Sends a UDP message to a specific address.
     * @param data Pointer to the data to send.
     * @param size Size of the data in bytes.
     * @param clientAddr Destination address.
     */
    void sendTo(const void* data, size_t size, const sockaddr_in& clientAddr);

    /**
     * @brief Converts a sockaddr_in to an asio UDP endpoint.
     * @param in The sockaddr_in structure.
     * @return The corresponding asio::ip::udp::endpoint.
     */
    static asio::ip::udp::endpoint sockaddrToEndpoint(const sockaddr_in& in);
    
    /**
     * @brief Converts an asio UDP endpoint to a sockaddr_in.
     * @param ep The asio::ip::udp::endpoint.
     * @return The corresponding sockaddr_in structure.
     */
    static sockaddr_in endpointToSockaddr(const asio::ip::udp::endpoint& ep);

private:
    asio::io_context ioContext;
    asio::ip::udp::socket socket;
};

