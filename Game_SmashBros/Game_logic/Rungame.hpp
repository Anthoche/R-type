/**
 * @file ServerGame.hpp
 * @brief Server-side game logic and state management for multiplayer sessions
 */

#pragma once

#include "../../Server/Include/IServerGame.hpp"
#include <asio/ip/udp.hpp>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <chrono>
#include <mutex>
#include <vector>
#include <queue>
#include <map>
#include <optional>

/**
 * @class ServerGame
 * @brief Core server game instance managing multiplayer game sessions
 *
 * Handles all server-side game logic including:
 * - Player state synchronization and updates
 * - Physics simulation (gravity, collisions, movement)
 * - Combat system (attacks, damage, knockback)
 * - Game rules (death, respawn, win conditions)
 * - Network message processing and broadcasting
 * - Entity management (players, obstacles, platforms)
 * - Score tracking (individual and global)
 * 
 * The server runs a fixed-tick game loop that processes inputs,
 * updates game state, and broadcasts changes to all clients.
 */
class ServerGame : public IServerGame {
    public:
        /**
         * @brief Constructs a server game instance
         * @param conn Reference to the network connection handler
         */
        explicit ServerGame(Connexion &conn);

        /**
         * @brief Starts the game loop for a specific room
         * @param roomId Unique identifier of the game room
         * 
         * Initializes game state and begins the fixed-tick update loop
         * that continues until the game ends or is interrupted.
         */
        void run(int roomId) override;
        
        /**
         * @brief Enqueues a network packet for processing
         * @param data Raw packet data received from client
         * @param from Endpoint address of the sender
         * 
         * Thread-safe method to queue incoming packets for processing
         * in the game loop thread.
         */
        void enqueuePacket(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) override;
        
        /**
         * @brief Sets the initial client list for the game session
         * @param clients Map of client IDs to their ready status
         * 
         * Called before game start to establish which clients are
         * participating in this game session.
         */
        void setInitialClients(const std::map<uint32_t, bool> &clients) override;

        /**
         * @brief Loads player configurations from file
         * @param path File path to player configuration data
         * 
         * Initializes player entities with stats, spawn positions,
         * and other configuration from the specified file.
         */
        void load_players(const std::string &path);

        /**
         * @brief Loads level data from file
         * @param path File path to level configuration data
         * 
         * Creates obstacles, platforms, and other level geometry
         * from the specified level file.
         */
        void load_level(const std::string &path);

        /**
         * @brief Sends complete game state to a specific client
         * @param clientId Target client identifier
         * 
         * Broadcasts all entities and their current states to a client,
         * typically used for newly connected or reconnecting players.
         */
        void broadcast_full_registry_to(uint32_t clientId);

    private:
        Connexion &connexion; ///< Network connection handler

        ecs::registry registry_server; ///< Entity component system registry

        std::unordered_map<uint32_t, SceneState> clientScenes; ///< Current scene state for each client

        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions; ///< Player positions (x, y)

        std::unordered_map<uint32_t, float> playerVelocities; ///< Vertical velocities for gravity
        std::unordered_map<uint32_t, float> playerHorizontalKnockback; ///< Active horizontal knockback values
        std::unordered_map<uint32_t, float> playerVerticalKnockback; ///< Active vertical knockback values
        std::unordered_map<uint32_t, int> playerLives; ///< Remaining lives for each player
        std::unordered_map<uint32_t, std::pair<float, float>> playerSpawnPositions; ///< Respawn coordinates

        std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> playerDamageCooldown; ///< Invulnerability timer after taking damage
        std::unordered_map<uint32_t, int> playerJumpCount; ///< Current jump count (for double/multi-jump)

        /**
         * @struct InputEvent
         * @brief Represents a single input state change
         */
        struct InputEvent {
            InputCode code;  ///< The input key/button
            bool pressed;    ///< Whether it was pressed (true) or released (false)
        };

        /**
         * @struct PlayerInputState
         * @brief Tracks the current state of all player inputs
         */
        struct PlayerInputState {
            bool up{false};      ///< Up/jump input state
            bool down{false};    ///< Down/crouch input state
            bool left{false};    ///< Left movement input state
            bool right{false};   ///< Right movement input state
            bool j{false};       ///< J key action state
            bool k{false};       ///< K key action state
            int lastDirX{1};     ///< Last horizontal direction (-1 left, 1 right)
            int lastDirY{0};     ///< Last vertical direction (-1 down, 0 neutral, 1 up)
        };

        std::unordered_map<uint32_t, std::vector<InputEvent>> playerInputBuffers; ///< Queued input events per player
        std::unordered_map<uint32_t, PlayerInputState> playerInputStates; ///< Current input state per player

        std::mutex mtx; ///< General purpose mutex for thread safety

        std::unordered_set<uint32_t> deadPlayers; ///< Set of currently dead player IDs

        std::unordered_map<uint32_t, int> playerIndividualScores; ///< Individual score per player

        int totalScore = 0; ///< Combined team/global score

        std::vector<ecs::entity_t> _obstacles; ///< List of obstacle entities
        std::vector<ecs::entity_t> _platforms; ///< List of platform entities
        int _roomId = -1; ///< Current room identifier

        /**
         * @brief Initializes starting positions for all players
         */
        void initialize_player_positions();
        
        /**
         * @brief Sets up initial game state and entities
         */
        void setup_game_state();
        
        /**
         * @brief Main game update loop iteration
         * @param tickMs Milliseconds per tick
         * @param dt Delta time in seconds
         */
        void game_loop(int tickMs, float dt);
        
        /**
         * @brief Updates player-specific logic
         * @param dt Delta time in seconds
         */
        void service_players(float dt);
        
        /**
         * @brief Broadcasts player position updates to all clients
         */
        void broadcast_player_updates();
        
        /**
         * @brief Indexes all existing entities for quick lookup
         */
        void index_existing_entities();
        
        /**
         * @brief Initializes obstacle collision data
         */
        void initialize_obstacles();

        /**
         * @brief Processes all queued network packets
         */
        void process_pending_messages();
        
        /**
         * @brief Handles a single client message
         * @param data Raw message data
         * @param from Sender endpoint
         */
        void handle_client_message(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);
        
        /**
         * @brief Processes client input message
         * @param msg Parsed input message
         */
        void handle_client_input_message(const ClientInputMessage &msg);
        
        /**
         * @brief Processes client scene state change
         * @param msg Parsed scene state message
         */
        void handle_scene_state_message(const SceneStateMessage &msg);
        
        /**
         * @brief Processes initial health configuration message
         * @param msg Parsed health message
         */
        void handle_initial_health_message(const InitialHealthMessage &msg);
        
        /**
         * @brief Checks if an input code should be tracked
         * @param code Input code to check
         * @return True if this input affects gameplay
         */
        bool is_tracked_input(InputCode code) const;
        
        /**
         * @brief Processes a single input event for a player
         * @param clientId Player identifier
         * @param state Current input state
         * @param event Input event to process
         */
        void process_input_event(uint32_t clientId, PlayerInputState &state, const InputEvent &event);
        
        /**
         * @brief Updates last known direction for a player
         * @param state Input state to update
         * @param x Horizontal direction
         * @param y Vertical direction
         */
        void set_last_direction(PlayerInputState &state, int x, int y);
        
        /**
         * @brief Resolves attack input and triggers combat logic
         * @param clientId Attacking player ID
         * @param state Current input state
         * @param code Attack input code
         */
        void resolve_attack_input(uint32_t clientId, PlayerInputState &state, InputCode code);
        
        /**
         * @brief Handles horizontal player movement
         * @param clientId Player identifier
         * @param dt Delta time
         * @param state Input state
         * @param pos Position to update (in/out)
         * @param width Player width for collision
         * @param height Player height for collision
         */
        void move_player_horizontally(uint32_t clientId, float dt, PlayerInputState &state,
                                     std::pair<float, float> &pos, float width, float height);
        
        /**
         * @brief Applies horizontal knockback physics
         * @param clientId Player identifier
         * @param dt Delta time
         * @param pos Position to update (in/out)
         * @param width Player width for collision
         * @param height Player height for collision
         */
        void apply_horizontal_knockback(uint32_t clientId, float dt, std::pair<float, float> &pos,
                                        float width, float height);
        
        /**
         * @brief Processes all queued player inputs
         * @param dt Delta time
         */
        void process_player_inputs(float dt);
        
        /**
         * @brief Applies gravity to all players
         * @param dt Delta time
         */
        void apply_gravity(float dt);
        
        /**
         * @brief Checks if any players have fallen into death zones
         */
        void check_death_zone();
        
        /**
         * @brief Checks if win conditions are met
         */
        void check_win_condition();
        
        /**
         * @brief Finds the last remaining alive player
         * @return Client ID of last alive player, or nullopt if none/multiple
         */
        std::optional<uint32_t> find_last_alive_player();
        
        /**
         * @brief Handles victory condition for a player
         * @param winnerId Victorious player's client ID
         */
        void handle_victory(uint32_t winnerId);
        
        /**
         * @brief Executes a melee attack
         * @param attackerId Attacking player ID
         * @param range Attack range
         * @param damage Damage to deal
         * @param baseKnockback Base knockback force
         * @param knockbackScale Knockback scaling factor
         * @param dirX Horizontal attack direction
         * @param dirY Vertical attack direction
         */
        void handle_melee_attack(uint32_t attackerId, float range, int damage, float baseKnockback, float knockbackScale, int dirX, int dirY);
        
        /**
         * @struct PlayerCombatInfo
         * @brief Combat-relevant information for a player
         */
        struct PlayerCombatInfo {
            float width{30.f};   ///< Player hitbox width
            float height{30.f};  ///< Player hitbox height
            std::size_t index{0}; ///< Registry index
        };
        
        /**
         * @struct MeleeTarget
         * @brief Information about a melee attack target
         */
        struct MeleeTarget {
            uint32_t id{0};       ///< Target client ID
            std::size_t index{0}; ///< Target registry index
            bool horizontal{true}; ///< Whether attack is primarily horizontal
            float distance{0.f};  ///< Distance to target
        };
        
        /**
         * @brief Retrieves combat info for a player
         * @param clientId Player identifier
         * @return Combat info if player exists, nullopt otherwise
         */
        std::optional<PlayerCombatInfo> get_player_info(uint32_t clientId);
        
        /**
         * @brief Selects the best target for a melee attack
         * @param attackerId Attacking player ID
         * @param range Attack range
         * @param dirX Horizontal direction
         * @param dirY Vertical direction
         * @param attackerInfo Attacker's combat info
         * @param attackerX Attacker's X position
         * @param attackerY Attacker's Y position
         * @return Target info if valid target found, nullopt otherwise
         */
        std::optional<MeleeTarget> select_melee_target(uint32_t attackerId, float range, int dirX, int dirY,
                                                       const PlayerCombatInfo &attackerInfo, float attackerX, float attackerY);
        
        /**
         * @brief Applies damage and knockback to a melee target
         * @param attackerId Attacking player ID
         * @param target Target information
         * @param damage Damage to apply
         * @param baseKnockback Base knockback force
         * @param knockbackScale Knockback scaling
         * @param dirX Horizontal direction
         * @param dirY Vertical direction
         */
        void apply_melee_damage(uint32_t attackerId, const MeleeTarget &target, int damage,
                                float baseKnockback, float knockbackScale, int dirX, int dirY);
        
        /**
         * @brief Handles player defeat logic
         * @param clientId Defeated player ID
         * @param registryIndex Player's registry index
         * @return True if player is permanently out, false if respawning
         */
        bool handle_player_defeat(uint32_t clientId, std::size_t registryIndex);
        
        /**
         * @brief Respawns a player at their spawn point
         * @param clientId Player identifier
         * @param registryIndex Player's registry index
         */
        void respawn_player(uint32_t clientId, std::size_t registryIndex);
        
        /**
         * @brief Ensures player has tracking data initialized
         * @param clientId Player identifier
         */
        void ensure_player_tracking(uint32_t clientId);
        
        /**
         * @brief Computes safe respawn position for a player
         * @param clientId Player identifier
         * @param playerWidth Player width
         * @param playerHeight Player height
         * @return Safe respawn coordinates (x, y)
         */
        std::pair<float, float> compute_respawn_position(uint32_t clientId, float playerWidth, float playerHeight);
        
        /**
         * @brief Gets player dimensions
         * @param clientId Player identifier
         * @return Width and height pair
         */
        std::pair<float, float> get_player_dimensions(uint32_t clientId);

        /**
         * @brief Checks if player is standing on ground or platform
         * @param clientId Player identifier
         * @param x Player X position
         * @param y Player Y position
         * @param playerWidth Player width
         * @param playerHeight Player height
         * @return True if on solid ground
         */
        bool is_on_ground(uint32_t clientId, float x, float y, float playerWidth, float playerHeight);
        
        /**
         * @brief Snaps player position to platform top
         * @param x Player X position
         * @param y Player Y position
         * @param playerWidth Player width
         * @param playerHeight Player height
         * @return Adjusted Y position on platform top
         */
        float snap_to_platform_top(float x, float y, float playerWidth, float playerHeight);

        /**
         * @brief Checks for player-enemy collisions and applies damage
         */
        void check_player_enemy_collisions();
        
        /**
         * @brief Checks if two axis-aligned bounding boxes overlap
         * @param left1 Left edge of box 1
         * @param right1 Right edge of box 1
         * @param top1 Top edge of box 1
         * @param bottom1 Bottom edge of box 1
         * @param left2 Left edge of box 2
         * @param right2 Right edge of box 2
         * @param top2 Top edge of box 2
         * @param bottom2 Bottom edge of box 2
         * @return True if boxes overlap
         */
        bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                                float left2, float right2, float top2, float bottom2);
        
        /**
         * @brief Checks if a position is blocked by obstacles
         * @param testX X position to test
         * @param testY Y position to test
         * @param playerWidth Player width
         * @param playerHeight Player height
         * @param obstacles List of obstacle entities
         * @return True if position is blocked
         */
        bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                                const std::vector<ecs::entity_t> &obstacles);
        
        /**
         * @brief Checks if position is blocked by platforms
         * @param clientId Player identifier
         * @param testX X position to test
         * @param testY Y position to test
         * @param playerWidth Player width
         * @param playerHeight Player height
         * @param platforms List of platform entities
         * @param movingDown Whether player is moving downward
         * @param isDownPressed Whether down input is pressed
         * @return True if position is blocked
         */
        bool is_position_blocked_platform(uint32_t clientId, float testX, float testY, float playerWidth, float playerHeight,
                                         const std::vector<ecs::entity_t> &platforms, bool movingDown, bool isDownPressed = false);
        
        /**
         * @brief Gets registry index for a client ID
         * @param clientId Player identifier
         * @return Registry index
         */
        std::size_t registry_index_for_client(uint32_t clientId);
        
        /**
         * @brief Finalizes player death (no more lives)
         * @param clientId Player identifier
         * @param registryIndex Player's registry index
         * @return True if player is permanently dead
         */
        bool finalize_player_death(uint32_t clientId, std::size_t registryIndex);
        
        /**
         * @brief Resets all player motion values to zero
         * @param clientId Player identifier
         */
        void reset_player_motion(uint32_t clientId);
        
        /**
         * @brief Handles jump input request
         * @param clientId Player identifier
         * @param onGround Whether player is on ground
         * @param velocity Vertical velocity to modify (in/out)
         * @param state Input state
         * @param jumpCount Jump counter to update (in/out)
         */
        void handle_jump_request(uint32_t clientId, bool onGround, float &velocity,
                                 PlayerInputState &state, int &jumpCount);
        
        /**
         * @brief Integrates vertical motion and gravity
         * @param clientId Player identifier
         * @param dt Delta time
         * @param pos Position to update (in/out)
         * @param playerWidth Player width
         * @param playerHeight Player height
         * @param velocity Vertical velocity (in/out)
         * @param state Input state
         */
        void integrate_vertical_motion(uint32_t clientId, float dt, std::pair<float, float> &pos,
                                       float playerWidth, float playerHeight, float &velocity,
                                       PlayerInputState &state);
        
        /**
         * @brief Helper to safely get component pointer
         * @tparam Component Component type
         * @param registry ECS registry
         * @param entity Entity to query
         * @return Pointer to component or nullptr if not found
         */
        template <typename Component>
        static Component *component_ptr(ecs::registry &registry, ecs::entity_t entity) {
            auto &arr = registry.get_components<Component>();
            auto idx = static_cast<std::size_t>(entity);
            if (idx >= arr.size() || !arr[idx].has_value())
                return nullptr;
            return &arr[idx].value();
        }

        /**
         * @struct PendingPacket
         * @brief Queued network packet awaiting processing
         */
        struct PendingPacket {
            std::vector<uint8_t> data;       ///< Raw packet data
            asio::ip::udp::endpoint endpoint; ///< Sender address
        };

        std::mutex packetMutex; ///< Mutex for packet queue thread safety
        std::queue<PendingPacket> pendingPackets; ///< Queue of incoming packets
        std::mutex initialClientsMutex; ///< Mutex for initial clients map
        std::map<uint32_t, bool> initialClients; ///< Map of initial client IDs to ready status

        /**
         * @brief Broadcasts game state updates to all clients
         */
        void broadcast_states_to_clients();
        
        /**
         * @brief Broadcasts player death notification
         * @param clientId Dead player's ID
         */
        void broadcast_player_death(uint32_t clientId);
        
        /**
         * @brief Broadcasts health updates for all players
         */
        void broadcast_player_health();
        
        /**
         * @brief Broadcasts global team score
         */
        void broadcast_global_score();
        
        /**
         * @brief Broadcasts individual player scores
         */
        void broadcast_individual_scores();
        
        /**
         * @brief Broadcasts game winner announcement
         * @param winnerId Winning player's ID
         */
        void broadcast_game_winner(uint32_t winnerId);

        /**
         * @brief Broadcasts obstacle spawn event
         * @param obstacleId Obstacle entity ID
         * @param x X position
         * @param y Y position
         * @param z Z position/layer
         * @param w Width
         * @param h Height
         * @param d Depth
         */
        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float z, float w, float h, float d);
        
        /**
         * @brief Broadcasts obstacle despawn event
         * @param obstacleId Obstacle entity ID to remove
         */
        void broadcast_obstacle_despawn(uint32_t obstacleId);

        /**
         * @brief Sleeps to maintain consistent tick rate
         * @param start Tick start time
         * @param tick_ms Target tick duration in milliseconds
         */
        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point &start, int tick_ms);
};