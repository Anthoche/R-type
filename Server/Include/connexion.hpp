/*
** EPITECH PROJECT, 2025
** G-CCP-500-PAR-5-1-rtype-1
** File description:
** connexion
*/

#pragma once

#include "../../Shared/Sockets/Include/UDP_socket.hpp"
#include "../../Shared/Sockets/Include/TCP_socket.hpp"
#include "../../Shared/protocol.hpp"
#include "../../Server/Room/Room.hpp"
#include <asio.hpp>
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class Room;

/**
 * @class Connexion
 * @brief Manages network connections for the R-Type game server.
 *
 * This class provides a high-level interface for handling both UDP and TCP connections.
 * It manages client registration, message transmission (unicast and broadcast),
 * and asynchronous packet reception. The class supports:
 * - Fast UDP communication for game state updates
 * - Reliable TCP communication for critical data
 * - Client lifecycle management (registration and disconnection)
 * - Room-based message broadcasting
 */
class Connexion {
    public:
        /**
         * @struct ReceivedPacket
         * @brief Contains data from a received UDP packet and its source endpoint.
         */
        struct ReceivedPacket {
            std::vector<uint8_t> data;           ///< Raw packet data.
            asio::ip::udp::endpoint endpoint;    ///< Source address and port.
        };

        /**
         * @brief Constructs a Connexion instance and binds to a UDP port.
         *
         * Initializes the UDP socket and prepares the connection manager for
         * handling client communications.
         *
         * @param io ASIO io_context for asynchronous operations.
         * @param port UDP port number to bind to.
         */
        Connexion(asio::io_context& io, uint16_t port);

        /**
         * @brief Destructor that cleans up network resources.
         *
         * Stops listening threads and closes all client connections.
         */
        ~Connexion();

        /**
         * @brief Sends a UDP message to a specific client.
         *
         * @param msg Pointer to the data buffer to send.
         * @param size Size of the data in bytes.
         * @param to Destination UDP endpoint (address and port).
         */
        void sendTo(const void* msg, size_t size, const asio::ip::udp::endpoint& to);

        /**
         * @brief Broadcasts a UDP message to all connected clients.
         *
         * Sends the same message to every registered client endpoint.
         *
         * @param msg Pointer to the data buffer to broadcast.
         * @param size Size of the data in bytes.
         */
        void broadcast(const void* msg, size_t size);

        /**
         * @brief Broadcasts a UDP message to all clients in a specific room.
         *
         * Sends the message only to clients who are members of the specified room.
         *
         * @param room The room whose members will receive the message.
         * @param data Pointer to the data buffer to broadcast.
         * @param size Size of the data in bytes.
         */
        void broadcastToRoom(Room const &room, const void* data, size_t size);

        /**
         * @brief Registers a new client with the connection manager.
         *
         * Adds the client to the tracking system, associating their endpoint with a unique ID.
         *
         * @param endpoint UDP endpoint of the client (address and port).
         * @param id Unique identifier assigned to this client.
         */
        void addClient(const asio::ip::udp::endpoint& endpoint, uint32_t id);

        /**
         * @brief Disconnects a client and removes them from the manager.
         *
         * Cleans up all resources associated with the client, including UDP and TCP connections.
         *
         * @param id Unique identifier of the client to disconnect.
         */
        void disconnectClient(uint32_t id);

        /**
         * @brief Gets the number of currently connected clients.
         *
         * @return Total count of active client connections.
         */
        size_t getClientCount() const;

        /**
         * @brief Gets the map of client addresses to their IDs.
         *
         * @return Const reference to the internal clients map.
         */
        const std::unordered_map<std::string, uint32_t>& getClients() const;

        /**
         * @brief Gets the map of client addresses to their UDP endpoints.
         *
         * @return Const reference to the internal endpoints map.
         */
        const std::unordered_map<std::string, asio::ip::udp::endpoint>& getEndpoints() const;

        /**
         * @brief Accepts a TCP connection from a client on a specific port.
         *
         * Establishes a reliable TCP connection for sending critical game data.
         *
         * @param id Client ID to associate with the TCP connection.
         * @param port TCP port to listen on for this client.
         * @return true if the connection was established successfully, false otherwise.
         */
        bool acceptTcpClient(uint32_t id, uint16_t port);

        /**
         * @brief Sends a JSON object to a specific TCP client.
         *
         * Uses the reliable TCP connection to transmit structured data.
         *
         * @param id Client ID of the recipient.
         * @param j JSON object to send.
         */
        void sendJsonToClient(uint32_t id, const nlohmann::json& j);

        /**
         * @brief Broadcasts a JSON object to all connected TCP clients.
         *
         * Sends the same JSON data to every client with an active TCP connection.
         *
         * @param j JSON object to broadcast.
         */
        void broadcastJson(const nlohmann::json& j);

        /**
         * @brief Starts the asynchronous packet reception thread.
         *
         * Begins listening for incoming UDP packets in the background.
         */
        void startListening();

        /**
         * @brief Stops the packet reception thread.
         *
         * Halts the background listener and processes any remaining queued packets.
         */
        void stopListening();

        /**
         * @brief Waits for a packet to arrive with a timeout.
         *
         * Blocks until a packet is received or the timeout expires.
         *
         * @param packet Output parameter that receives the packet data.
         * @param timeout Maximum time to wait for a packet.
         * @return true if a packet was received, false if the timeout expired.
         */
        bool waitForPacket(ReceivedPacket &packet, std::chrono::milliseconds timeout);

        /**
         * @brief Attempts to retrieve a packet from the queue without blocking.
         *
         * @param packet Output parameter that receives the packet data if available.
         * @return true if a packet was retrieved, false if the queue was empty.
         */
        bool tryPopPacket(ReceivedPacket &packet);

        /**
         * @brief Sends a UDP message to a specific set of clients.
         *
         * Broadcasts to a subset of clients identified by their IDs.
         *
         * @param clientIds Vector of client IDs to send the message to.
         * @param data Pointer to the data buffer to send.
         * @param size Size of the data in bytes.
         */
        void broadcastToClients(const std::vector<uint32_t> &clientIds, const void* data, size_t size);

        /**
         * @brief Associates a display name with a connected client.
         *
         * Stores a human-readable name for the client, useful for chat and UI.
         *
         * @param id Client ID.
         * @param name Display name to associate with the client.
         */
        void setClientName(uint32_t id, std::string name);

        /**
         * @brief Retrieves the stored display name for a client.
         *
         * @param id Client ID to look up.
         * @return The client's display name, or an empty string if not found.
         */
        [[nodiscard]] std::string getClientName(uint32_t id) const;

    private:
        UDP_socket socket; ///< UDP socket for fast, unreliable message transmission.
        std::unordered_map<std::string, uint32_t> clients; ///< Maps client endpoint strings to their unique IDs.
        std::unordered_map<std::string, asio::ip::udp::endpoint> endpoints; ///< Maps client endpoint strings to UDP endpoint objects.
        std::unordered_map<uint32_t, std::shared_ptr<TCP_socket>> tcpClients; ///< Maps client IDs to their TCP connections.
        std::unordered_map<uint32_t, std::string> clientNames; ///< Maps client IDs to their display names.
        std::atomic<bool> listening{false}; ///< Flag indicating whether the receiver thread is active.
        std::thread receiverThread; ///< Background thread for receiving UDP packets.
        std::mutex queueMutex; ///< Protects access to the packet queue.
        std::condition_variable queueCv; ///< Notifies waiting threads when packets arrive.
        std::queue<ReceivedPacket> packetQueue; ///< Queue of received packets waiting to be processed.

        /**
         * @brief Main loop for the receiver thread.
         *
         * Continuously receives UDP packets and adds them to the queue.
         */
        void receiverLoop();
};