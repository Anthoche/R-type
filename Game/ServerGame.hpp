/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/
#pragma once

#include <unordered_map>
#include <tuple>
#include <utility>
#include <cstdint>
#include <chrono>
#include <vector>
#include <sys/socket.h>
#include <nlohmann/json.hpp>
#include "../Shared/protocol.hpp"
#include "../Server/Network_handler/Include/UDP_socket.hpp"
#include "Entities/Include/components.hpp"
#include "Entities/Include/entity_storage.hpp"

class ServerGame {
    UDP_socket &socket; ///< Reference to the UDP socket used for communication.
    std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Player positions keyed by player ID.
    std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles; ///< Obstacles keyed by ID (x,y,w,h).
    ecs::registry reg; ///< Registre ECS pour stocker les entités du jeu.
public:
    /**
     * @brief Construct a new ServerGame.
     * @param sock Reference to the UDP socket used for client communication.
     */
    explicit ServerGame(UDP_socket &sock);

    /**
     * @brief Run the main server game loop.
     */
    void run();

    /**
     * @brief Charge les joueurs depuis un fichier JSON et les stocke dans le registre ECS.
     * @param json_path Chemin vers le fichier JSON contenant les données des joueurs.
     */
    void load_players_from_json(const std::string &json_path);

private:
    /**
     * @brief Initialize player positions at the start of the game.
     */
    void initialize_player_positions();

    /**
     * @brief Initialize obstacles at the start of the game.
     */
    void initialize_obstacles();

    /**
     * @brief Process any pending network messages from clients.
     */
    void process_pending_messages();

    /**
     * @brief Handle a single message from a client.
     * @param data Raw message data received from the client.
     * @param from Client network address.
     */
    void handle_client_message(const std::vector<uint8_t>& data, const sockaddr_in& from);

    /**
     * @brief Broadcast the current state of all entities to connected clients.
     */
    void broadcast_states_to_clients();

    /**
     * @brief Notify clients that an obstacle has spawned.
     * @param obstacleId Unique ID of the obstacle.
     * @param x X-coordinate of the obstacle.
     * @param y Y-coordinate of the obstacle.
     * @param w Width of the obstacle.
     * @param h Height of the obstacle.
     */
    void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);

    /**
     * @brief Notify clients that an obstacle has been removed.
     * @param obstacleId ID of the despawned obstacle.
     */
    void broadcast_obstacle_despawn(uint32_t obstacleId);

    /**
     * @brief Sleep to maintain a fixed tick rate for the server loop.
     * @param start Start time of the current tick.
     * @param tick_ms Duration of one tick in milliseconds.
     */
    void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};
