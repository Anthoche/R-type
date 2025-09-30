/*
** EPITECH PROJECT, 2025
** G-CCP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#pragma once

#include "UDP_socket.hpp"
#include "../../../Shared/protocol.hpp"
#include <vector>
#include <unordered_map>
#include <asio.hpp>
#include <functional>
#include <iostream>

/**
 * @brief High-level connection manager for UDP network operations.
 *
 * Wraps around UDP_socket, providing simplified API for receiving,
 * sending, broadcasting messages, and client management.
 */
class Connexion {
public:
    /**
     * @brief Constructs a Connexion instance bound to the specified port.
     * @param io Reference to the Asio io_context (shared by the app).
     * @param port The UDP port to listen on.
     */
    Connexion(asio::io_context& io, uint16_t port);

    ~Connexion() = default;

    /**
     * @brief Starts asynchronous reception of UDP messages.
     * @param handler Callback taking (error_code, data, sender_endpoint).
     */
    void asyncReceive(std::function<void(const asio::error_code&, std::vector<uint8_t>, asio::ip::udp::endpoint)> handler);

    /**
     * @brief Sends a UDP message to a specific client.
     * @param msg Pointer to the message data.
     * @param size Size of the message in bytes.
     * @param to Destination endpoint.
     */
    void sendTo(const void* msg, size_t size, const asio::ip::udp::endpoint& to);
    /**
     * @brief Broadcasts a UDP message to all connected clients.
     * @param msg Pointer to the message data.
     * @param size Size of the message in bytes.
     */
    void broadcast(const void* msg, size_t size);

    /**
     * @brief Registers a new client with the connection manager.
     * @param endpoint Address of the client to add.
     * @param id Unique ID to assign to the client.
     */
    void addClient(const asio::ip::udp::endpoint& endpoint, uint32_t id);
    /**
     * @brief Gets the number of connected clients.
     * @return Current client count.
     */
    size_t getClientCount() const;

    /**
     * @brief Gets the map of connected clients (IP:port → ID).
     */
    const std::unordered_map<std::string, uint32_t>& getClients() const;

    /**
     * @brief Gets the map of endpoints (IP:port → udp::endpoint).
     */
    const std::unordered_map<std::string, asio::ip::udp::endpoint>& getEndpoints() const;
private:
    UDP_socket socket; ///< Underlying UDP socket for network operations.
    std::unordered_map<std::string, uint32_t> clients; ///< Maps client address strings to their assigned IDs.
    std::unordered_map<std::string, asio::ip::udp::endpoint> endpoints; ///< Maps address strings to actual endpoints for sending.
};
