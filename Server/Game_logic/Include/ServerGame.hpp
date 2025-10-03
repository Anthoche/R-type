/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/
#pragma once
#include "../../../Shared/protocol.hpp"
#include "../../../Engine/Core/Include/registry.hpp"
#include "../../../Engine/Utils/Include/entity_storage.hpp"
#include "../../Network_handler/Include/connexion.hpp"
#include <asio/ip/udp.hpp>
#include <unordered_map>
#include <string>
#include <chrono>
#include <mutex>

/**
 * @class ServerGame
 * @brief Main server-side game logic handler.
 *
 * Manages the game loop, player states, ECS registry, and network broadcasting.
 */
class ServerGame {
    public:
        /**
         * @brief Constructs a ServerGame instance.
         * @param conn Reference to the Connexion handler for network communication.
         */
        ServerGame(Connexion &conn);

        /**
         * @brief Runs the main server game loop.
         *
         * Handles message processing, state updates, and broadcasting.
         */
        void run();

        /**
         * @brief Loads players from a JSON configuration file.
         * @param path Path to the players JSON file.
         */
        void load_players(const std::string &path);

        /**
         * @brief Loads level entities from a JSON configuration file.
         * @param path Path to the level JSON file.
         */
        void load_level(const std::string &path);

        /**
         * @brief Broadcasts the entire ECS registry to all clients.
         *
         * Serializes all entities and sends them in batches.
         */
        void broadcast_full_registry();

    private:
        Connexion &connexion;
        ecs::registry registry_server;
        std::unordered_map<uint32_t, SceneState> clientScenes;
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;
        std::unordered_map<uint32_t, std::tuple<float, float, float, float>> obstacles;
        std::mutex mtx;  // <-- Ajout pour thread safety

        /**
         * @brief Initializes player positions based on connected clients.
         */
        void initialize_player_positions();

        /**
         * @brief Processes pending network messages from clients.
         */
        void process_pending_messages();

        /**
         * @brief Handles a single client message.
         * @param data Raw message data.
         * @param from Client endpoint.
         */
        void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);  // <-- Changement ici

        /**
         * @brief Broadcasts player state updates to all clients.
         */
        void broadcast_states_to_clients();

        /**
         * @brief Initializes obstacles in the game world.
         */
        void initialize_obstacles();

        /**
         * @brief Broadcasts an obstacle spawn message.
         * @param obstacleId Unique obstacle ID.
         * @param x X position.
         * @param y Y position.
         * @param w Width.
         * @param h Height.
         */
        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float w, float h);

        /**
         * @brief Broadcasts an obstacle despawn message.
         * @param obstacleId Unique obstacle ID.
         */
        void broadcast_obstacle_despawn(uint32_t obstacleId);

        /**
         * @brief Sleeps to maintain consistent tick rate.
         * @param start Tick start time.
         * @param tick_ms Target tick duration in milliseconds.
         */
        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);
};
