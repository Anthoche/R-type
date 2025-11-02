/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** ServerGame
*/

#pragma once

#include "../../Server/Include/IServerGame.hpp"
#include <asio/ip/udp.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <mutex>
#include <queue>
#include <vector>

/**
 * @class ServerGame
 * @brief Implementation of IServerGame for R-Type multiplayer gameplay.
 *
 * This class handles:
 * - The main server game loop (~60 FPS).
 * - Player management, movement, shooting, and death.
 * - Enemy spawning, movement patterns, and AI projectiles.
 * - Collision detection between all entities.
 * - Broadcasting of all gameplay updates to clients.
 */
class ServerGame : public IServerGame {
    public:
        /**
         * @brief Constructs a ServerGame instance.
         * @param conn Reference to the Connexion handler for network communication.
         */
        explicit ServerGame(Connexion &conn);

        /**
         * @brief Main server game loop.
         * @param roomId The id of the room to listen on
         */
        void run(int roomId) override;

        /**
         * @brief Enqueues a received network packet for processing.
         * @param data The raw packet data.
         * @param from The sender's network endpoint.
         */
        void enqueuePacket(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) override;
        
        /**
         * @brief Sets the initial clients connected to the server.
         * @param clients Map of client IDs to their connection status.
         */
        void setInitialClients(const std::map<uint32_t, bool> &clients) override;

        /**
         * @brief Loads player data from a JSON configuration file.
         */
        void load_players(const std::string &path);

        /**
         * @brief Loads level entities from a JSON configuration file.
         */
        void load_level(const std::string &path);

        /**
         * @brief Sends the complete game registry state to a specific client.
         */
        void broadcast_full_registry_to(uint32_t clientId);

        /**
         * @brief Sets the initial player skins.
         * @param skins Map of client IDs to their skin filenames.
         */
        void setInitialPlayerSkins(const std::unordered_map<uint32_t, std::string> &skins);
        
        /**
         * @brief Sets the initial player weapons.
         * @param weapons Map of client IDs to their weapon identifiers.
         */
        void setInitialPlayerWeapons(const std::unordered_map<uint32_t, std::string> &weapons);

    private:

        /** @brief Maximum number of levels in the game. */
        static constexpr int MAX_LEVELS = 3;

        /** @brief Indicates if the game is in endless mode. */
        bool _isEndless = false;

        /** @brief Indicates if the game has been completed. */
        bool gameCompleted = false;

        /** @brief Maps enemy projectile IDs to their (x,y,z,velX,velY,velZ,ownerId). */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float, uint32_t>> enemyProjectiles;

        /** @brief Counter for assigning unique enemy projectile IDs. */
        uint32_t nextEnemyProjectileId = 100000;

        /** @brief Reference to the network connection handler. */
        Connexion &connexion;

        /** @brief ECS registry containing all server-side game entities and components. */
        ecs::registry registry_server;

        /** @brief Maps client IDs to their current scene state (MENU or GAME). */
        std::unordered_map<uint32_t, SceneState> clientScenes;

        /** @brief Maps player IDs to their (x, y) positions. Note: keeping 2D for backward compat, add z if needed */
        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;

        /** @brief Maps obstacle IDs to their (x, y, z, width, height, depth). */
        std::unordered_map<uint32_t, std::tuple<float, float, float, float, float, float>> obstacles;

        /** @brief Maps projectile IDs to their (x, y, z, velX, velY, velZ, ownerId). */
        struct ProjectileState {
            float x;
            float y;
            float z;
            float vx;
            float vy;
            float vz;
            uint32_t ownerId;
            float damage;
            float width;
            float height;
        };

        /** @brief Maps projectile IDs to their runtime state. */
        std::unordered_map<uint32_t, ProjectileState> projectiles;

        /** @brief Cooldown timestamps to avoid damage spam. */
        std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> playerDamageCooldown;

        /** @brief Structure to represent a player's input event. */
        struct InputEvent {
            InputCode code;
            bool pressed;
        };

        /** @brief Structure to represent the current input state of a player. */
        struct PlayerInputState {
            bool up{false};
            bool down{false};
            bool left{false};
            bool right{false};
        };

        /** @brief Buffers of input events per player. */
        std::unordered_map<uint32_t, std::vector<InputEvent>> playerInputBuffers;

        /** @brief Current input states per player. */
        std::unordered_map<uint32_t, PlayerInputState> playerInputStates;

        /** @brief Counter for assigning unique projectile IDs. */
        uint32_t nextProjectileId = 1;

        /** @brief Mutex for thread-safe access. */
        std::mutex mtx;

        /** @brief Mutex for thread-safe packet queue access. */
        std::mutex packetMutex;

        /** @brief Mutex for thread-safe access to initial clients. */
        mutable std::mutex initialClientsMutex;

        /** @brief Map of initial clients connected to the server. */
        std::map<uint32_t, bool> initialClients;

        /** @brief Set of dead player IDs. */
        std::unordered_set<uint32_t> deadPlayers;

        /** @brief Counter for assigning unique enemy IDs. */
        uint32_t nextEnemyId = 1;

        /** @brief Individual scores per player. */
        std::unordered_map<uint32_t, int> playerIndividualScores;

        /** @brief Cached skin filename per client. */
        std::unordered_map<uint32_t, std::string> _playerSkins;
        
        /** @brief Cached weapon identifier per client. */
        std::unordered_map<uint32_t, std::string> _playerWeapons;
        
        /** @brief Cooldown tracker per player weapon. */
        std::unordered_map<uint32_t, std::chrono::steady_clock::time_point> _playerLastShot;
        
        /** @brief Remaining ammo per player weapon (if applicable). */
        std::unordered_map<uint32_t, int> _playerAmmo;
        
        /** @brief Burst state tracker per player weapon (if applicable). */
        struct BurstState {
            std::chrono::steady_clock::time_point burstStart;
            std::chrono::steady_clock::time_point lastBurstEnd;
            bool inBurst{false};
        };
        /** @brief Burst states per player weapon. */
        std::unordered_map<uint32_t, BurstState> _playerBurstState;

        /** @brief Total cumulative score. */
        int totalScore = 0;

        /** @brief Enemy movement pattern registry. */
        std::unordered_map<uint32_t, std::string> enemyPatterns;

        /** @brief Cached references for obstacles. */
        std::vector<ecs::entity_t> _obstacles;

        /** @brief Cached references for enemies. */
        std::vector<ecs::entity_t> _enemies;

        /** @brief Cached references for elements. */
        std::vector<ecs::entity_t> _randomElements;

        /** @brief Current level number. */
        int currentLevel = 1;

        /** @brief Indicates if a level transition is pending. */
        bool levelTransitionPending = false;

        /** @brief Timestamp when level transition was initiated. */
        std::chrono::steady_clock::time_point levelTransitionTime;

        /** @brief Delay before transitioning to the next level. */
        const float LEVEL_TRANSITION_DELAY = 6.0f;

        /** @brief The id of the room to listen on */
        int _roomId;

        /** @brief Structure representing a pending network packet. */
        struct PendingPacket {
            std::vector<uint8_t> data;
            asio::ip::udp::endpoint endpoint;
        };

        /** @brief Queue of pending network packets to process. */
        std::queue<PendingPacket> pendingPackets;

        /**
        * @brief Initializes player starting positions. 
        */
        void initialize_player_positions();
        
        /** 
        * @brief Indexes existing entities in the ECS registry for quick access. 
        */
        void index_existing_entities();

        /**
        * @brief Processes all pending network messages.
        */
        void process_pending_messages();

        /**
         * @brief Handles an individual client message.
         * @param data The raw packet data.
         * @param from The sender's network endpoint.
         */
        void handle_client_message(const std::vector<uint8_t>& data, const asio::ip::udp::endpoint& from);

        /** 
         * @brief Broadcasts the current game states to all connected clients. 
         */
        void broadcast_states_to_clients();

        /** 
         * @brief Processes buffered player inputs and updates their states. 
         * @param dt Delta time since last update.
         */
        void check_player_enemy_collisions();

        /** 
         * @brief Broadcasts a player's death.
         * @param clientId The ID of the player who died.
         */
        void broadcast_player_death(uint32_t clientId);

        /** 
         * @brief Initializes obstacles in the game world. 
         */
        void initialize_obstacles();

        /** 
         * @brief Broadcasts obstacle spawn event to clients.
         * @param obstacleId The unique ID of the obstacle.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         * @param w Width.
         * @param h Height.
         * @param d Depth.
         * @param vx Velocity in X.
         * @param vy Velocity in Y.
         * @param vz Velocity in Z.
         */
        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float z, float w, float h, float d, float vx, float vy, float vz);
        
        /** 
         * @brief Broadcasts obstacle positions to clients.
         */
        void broadcast_obstacle_positions();

        /** 
         * @brief Broadcasts obstacle update event to clients.
         * @param obstacleId The unique ID of the obstacle.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         * @param vx Velocity in X.
         * @param vy Velocity in Y.
         * @param vz Velocity in Z.
         */
        void broadcast_obstacle_update(uint32_t obstacleId, float x, float y, float z,
                                          float vx, float vy, float vz);

        /** 
         * @brief Broadcasts obstacle despawn event to clients.
         * @param obstacleId The unique ID of the obstacle.
         */
        void broadcast_obstacle_despawn(uint32_t obstacleId);

        /** 
         * @brief Sleeps the thread to maintain a consistent tick rate.
         * @param start The start time of the tick.
         * @param tick_ms Desired tick duration in milliseconds.
         */
        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point& start, int tick_ms);

        /** 
         * @brief Updates all projectiles on the server side.
         * @param dt Delta time since last update.
         */
        void update_projectiles_server_only(float dt);

        /** 
         * @brief Broadcasts projectile positions to clients.
         */
        void broadcast_projectile_positions();

        /** 
         * @brief Checks for collisions between projectiles and other entities.
         */
        void check_projectile_collisions();

        /** 
         * @brief Broadcasts projectile spawn event to clients.
         * @param projId The unique ID of the projectile.
         * @param ownerId The ID of the entity that fired the projectile.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         * @param vx Velocity in X.
         * @param vy Velocity in Y.
         * @param vz Velocity in Z.
         */
        void broadcast_projectile_spawn(uint32_t projId, uint32_t ownerId, float x, float y, float z, float vx, float vy, float vz);
        
        /** 
         * @brief Broadcasts projectile despawn event to clients.
         * @param projId The unique ID of the projectile.
         */
        void broadcast_projectile_despawn(uint32_t projId);

        /** 
         * @brief Updates all enemies based on their movement patterns.
         * @param dt Delta time since last update.
         */
        void update_enemies(float dt);

        /** 
         * @brief Updates enemy with default movement pattern.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         */
        void update_enemy_default(uint32_t id, float dt);

        /** 
         * @brief Updates enemy with straight movement pattern.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         */
        void update_enemy_straight(uint32_t id, float dt);

        /** 
         * @brief Updates enemy with zigzag movement pattern.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         */
        void update_enemy_zigzag(uint32_t id, float dt);

        /** 
         * @brief Updates enemy with circular movement pattern.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         */
        void update_enemy_circle(uint32_t id, float dt);

        /** 
         * @brief Updates enemy with turret behavior.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         * @param rapidfire The rapidfire rate for the turret.
         */
        void update_enemy_turret(uint32_t id, float dt, float rapidfire);

        /** 
         * @brief Updates enemy with boss phase 1 behavior.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         */
        void update_enemy_boss(uint32_t id, float dt);

        /** 
         * @brief Updates enemy with figure-8 movement pattern.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         */
        void update_enemy_figure8(uint32_t id, float dt);

        /** 
         * @brief Updates enemy with spiral movement pattern.
         * @param id The unique ID of the enemy.
         * @param dt Delta time since last update.
         */
        void update_enemy_spiral(uint32_t id, float dt);

        /** 
         * @brief Updates all obstacles in the game world.
         * @param dt Delta time since last update.
         */
        void update_obstacles(float dt);

        /** 
         * @brief Updates all random elements in the game world.
         * @param dt Delta time since last update.
         */
        void update_element(float dt);

        /** 
         * @brief Broadcasts all element positions to clients.
         */
        void broadcast_element_positions();

        /** 
         * @brief Broadcasts element spawn event to clients.
         * @param elementId The unique ID of the element.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         * @param vx Velocity in X.
         * @param vy Velocity in Y.
         * @param vz Velocity in Z.
         * @param width Element width.
         * @param height Element height.
         */
        void broadcast_element_spawn(uint32_t elementId, float x, float y, float z, float vx, float vy, float vz, float width, float height);
        
        /** 
         * @brief Broadcasts element despawn event to clients.
         * @param elementId The unique ID of the element.
         */
        void broadcast_element_despawn(uint32_t elementId);
        
        /** 
         * @brief Broadcasts element update event to clients.
         * @param elementId The unique ID of the element.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         */
        void broadcast_element_update(uint32_t elementId, float x, float y, float z);

        /** 
         * @brief Broadcasts enemy spawn event to clients.
         * @param enemyId The unique ID of the enemy.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         * @param vx Velocity in X.
         * @param vy Velocity in Y.
         * @param vz Velocity in Z.
         * @param width Enemy width.
         * @param height Enemy height.
         */
        void broadcast_enemy_spawn(uint32_t enemyId, float x, float y, float z, float vx, float vy, float vz, float width, float height);
        
        /** 
         * @brief Broadcasts enemy positions to clients.
         */
        void broadcast_enemy_positions();

        /** 
         * @brief Broadcasts enemy update event to clients.
         * @param enemyId The unique ID of the enemy.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         */
        void broadcast_enemy_update(uint32_t enemyId, float x, float y, float z);

        /** 
         * @brief Broadcasts enemy despawn event to clients.
         * @param enemyId The unique ID of the enemy.
         */
        void broadcast_enemy_despawn(uint32_t enemyId);

        /** 
         * @brief Broadcasts boss death event to clients.
         * @param bossId The unique ID of the boss.
         */
        void broadcast_boss_death(uint32_t bossId);

        /** 
         * @brief Enemy shoots a projectile.
         * @param enemyId The unique ID of the enemy.
         * @param x X position of the projectile spawn.
         * @param y Y position of the projectile spawn.
         * @param vx Velocity in X.
         * @param vy Velocity in Y.
         */
        void shoot_enemy_projectile(uint32_t enemyId, float x, float y, float vx, float vy);

        /** 
         * @brief Updates all enemy projectiles on the server side.
         * @param dt Delta time since last update.
         */
        void update_enemy_projectiles_server_only(float dt);

        /** 
         * @brief Checks for collisions between enemy projectiles and players.
         */
        void check_enemy_projectile_player_collisions();

        /** 
         * @brief Checks for collisions between players and game elements.
         */
        void check_player_element_collisions();

        /** 
         * @brief Broadcasts enemy projectile spawn event to clients.
         * @param projId The unique ID of the projectile.
         * @param ownerId The ID of the entity that fired the projectile.
         * @param x X position.
         * @param y Y position.
         * @param z Z position.
         * @param vx Velocity in X.
         * @param vy Velocity in Y.
         * @param vz Velocity in Z.
         */
        void broadcast_enemy_projectile_spawn(uint32_t projId, uint32_t ownerId, float x, float y, float z, float vx, float vy, float vz);
        
        /** 
         * @brief Broadcasts enemy projectile positions to clients.
         */
        void broadcast_enemy_projectile_positions();

        /** 
         * @brief Broadcasts enemy projectile despawn event to clients.
         * @param projId The unique ID of the projectile.
         */
        void broadcast_enemy_projectile_despawn(uint32_t projId);

        /** 
         * @brief Broadcasts player health to all clients.
         */
        void broadcast_player_health();

        /** 
         * @brief Broadcasts the global score to all clients.
         */
        void broadcast_global_score();

        /** 
         * @brief Broadcasts endless mode status to all clients.
         * @param isEndless True if endless mode is active, false otherwise.
         */
        void broadcast_endless_mode(bool isEndless);

        /** 
         * @brief Broadcasts global health to all clients.
         * @param health The global health value.
         */
        void broadcast_global_health(int16_t health);

        /** 
         * @brief Broadcasts individual player scores to all clients.
         */
        void broadcast_individual_scores();

        /** 
         * @brief Checks for collisions between projectiles and enemies.
         */
        void check_projectile_enemy_collisions();

        /** 
         * @brief Loads the next level in the game.
         */
        void load_next_level();

        /** 
         * @brief Clears all entities related to the current level.
         */
        void clear_level_entities();

        /** 
         * @brief Processes buffered player inputs and updates their states.
         * @param dt Delta time since last update.
         */
        void process_player_inputs(float dt);
        
        /** 
         * @brief Checks for AABB overlap between two rectangles.
         * @param left1 Left edge of rectangle 1.
         * @param right1 Right edge of rectangle 1.
         * @param top1 Top edge of rectangle 1.
         * @param bottom1 Bottom edge of rectangle 1.
         * @param left2 Left edge of rectangle 2.
         * @param right2 Right edge of rectangle 2.
         * @param top2 Top edge of rectangle 2.
         * @param bottom2 Bottom edge of rectangle 2.
         * @return True if the rectangles overlap, false otherwise.
         */
        bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                                float left2, float right2, float top2, float bottom2);

        /** 
         * @brief Checks if a position is blocked by any obstacles.
         * @param testX X position to test.
         * @param testY Y position to test.
         * @param playerWidth Width of the player. 
         * @param playerHeight Height of the player.
         * @param obstacles List of obstacle entities to check against.
         * @return True if the position is blocked, false otherwise.
         */
        bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                                const std::vector<ecs::entity_t> &obstacles);

        /** 
        * @brief Broadcasts a player's skin selection to all clients.
        * @param clientId The ID of the player.
        * @param filename The filename of the selected skin.                       
        */                       
        void broadcast_player_skin(uint32_t clientId, const std::string &filename);

        /** 
        * @brief Sends the cached player skin to a specific client.
        * @param clientId The ID of the player.
        */
        void send_player_skins_to(uint32_t clientId);

        /** 
        * @brief Broadcasts a player's weapon selection to all clients.
        * @param clientId The ID of the player.
        * @param weaponId The identifier of the selected weapon.                       
        */
        void broadcast_player_weapon(uint32_t clientId, const std::string &weaponId);

        /** 
        * @brief Sends the cached player weapon to a specific client.
        * @param clientId The ID of the player.
        */
        void send_player_weapons_to(uint32_t clientId);

        /** 
        * @brief Collects the IDs of all clients in the current room.
        * @param includeDead Whether to include dead players in the list.
        * @return A vector of client IDs in the room.
        */
        std::vector<uint32_t> collectRoomClients(bool includeDead = true) const;
};
