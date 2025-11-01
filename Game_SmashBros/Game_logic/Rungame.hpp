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

class ServerGame : public IServerGame {
    public:
        explicit ServerGame(Connexion &conn);

        void run(int roomId) override;
        void enqueuePacket(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from) override;
        void setInitialClients(const std::map<uint32_t, bool> &clients) override;

        void load_players(const std::string &path);

        void load_level(const std::string &path);

        void broadcast_full_registry_to(uint32_t clientId);

    private:
        Connexion &connexion;

        ecs::registry registry_server;

        std::unordered_map<uint32_t, SceneState> clientScenes;

        std::unordered_map<uint32_t, std::pair<float, float>> playerPositions;

        std::unordered_map<uint32_t, float> playerVelocities;
        std::unordered_map<uint32_t, float> playerHorizontalKnockback;
        std::unordered_map<uint32_t, float> playerVerticalKnockback;
        std::unordered_map<uint32_t, int> playerLives;
        std::unordered_map<uint32_t, std::pair<float, float>> playerSpawnPositions;

        std::unordered_map<uint32_t, std::chrono::high_resolution_clock::time_point> playerDamageCooldown;
        std::unordered_map<uint32_t, int> playerJumpCount;

        struct InputEvent {
            InputCode code;
            bool pressed;
        };

        struct PlayerInputState {
            bool up{false};
            bool down{false};
            bool left{false};
            bool right{false};
            bool j{false};
            bool k{false};
            int lastDirX{1};
            int lastDirY{0};
        };

        std::unordered_map<uint32_t, std::vector<InputEvent>> playerInputBuffers;
        std::unordered_map<uint32_t, PlayerInputState> playerInputStates;

        std::mutex mtx;

        std::unordered_set<uint32_t> deadPlayers;

        std::unordered_map<uint32_t, int> playerIndividualScores;

        int totalScore = 0;

        std::vector<ecs::entity_t> _obstacles;
        std::vector<ecs::entity_t> _platforms;
        int _roomId = -1;

        void initialize_player_positions();
        void setup_game_state();
        void game_loop(int tickMs, float dt);
        void service_players(float dt);
        void broadcast_player_updates();
        void index_existing_entities();
        void initialize_obstacles();

        void process_pending_messages();
        void handle_client_message(const std::vector<uint8_t> &data, const asio::ip::udp::endpoint &from);
        void handle_client_input_message(const ClientInputMessage &msg);
        void handle_scene_state_message(const SceneStateMessage &msg);
        void handle_initial_health_message(const InitialHealthMessage &msg);
        bool is_tracked_input(InputCode code) const;
        void process_input_event(uint32_t clientId, PlayerInputState &state, const InputEvent &event);
        void set_last_direction(PlayerInputState &state, int x, int y);
        void resolve_attack_input(uint32_t clientId, PlayerInputState &state, InputCode code);
        void move_player_horizontally(uint32_t clientId, float dt, PlayerInputState &state,
                                     std::pair<float, float> &pos, float width, float height);
        void apply_horizontal_knockback(uint32_t clientId, float dt, std::pair<float, float> &pos,
                                        float width, float height);
        void process_player_inputs(float dt);
        void apply_gravity(float dt);
        void check_death_zone();
        void check_win_condition();
        std::optional<uint32_t> find_last_alive_player();
        void handle_victory(uint32_t winnerId);
        void handle_melee_attack(uint32_t attackerId, float range, int damage, float baseKnockback, float knockbackScale, int dirX, int dirY);
        struct PlayerCombatInfo {
            float width{30.f};
            float height{30.f};
            std::size_t index{0};
        };
        struct MeleeTarget {
            uint32_t id{0};
            std::size_t index{0};
            bool horizontal{true};
            float distance{0.f};
        };
        std::optional<PlayerCombatInfo> get_player_info(uint32_t clientId);
        std::optional<MeleeTarget> select_melee_target(uint32_t attackerId, float range, int dirX, int dirY,
                                                       const PlayerCombatInfo &attackerInfo, float attackerX, float attackerY);
        void apply_melee_damage(uint32_t attackerId, const MeleeTarget &target, int damage,
                                float baseKnockback, float knockbackScale, int dirX, int dirY);
        bool handle_player_defeat(uint32_t clientId, std::size_t registryIndex);
        void respawn_player(uint32_t clientId, std::size_t registryIndex);
        void ensure_player_tracking(uint32_t clientId);
        std::pair<float, float> compute_respawn_position(uint32_t clientId, float playerWidth, float playerHeight);
        std::pair<float, float> get_player_dimensions(uint32_t clientId);

        bool is_on_ground(uint32_t clientId, float x, float y, float playerWidth, float playerHeight);
        float snap_to_platform_top(float x, float y, float playerWidth, float playerHeight);

        void check_player_enemy_collisions();
        bool check_aabb_overlap(float left1, float right1, float top1, float bottom1,
                                float left2, float right2, float top2, float bottom2);
        bool is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                                const std::vector<ecs::entity_t> &obstacles);
        bool is_position_blocked_platform(uint32_t clientId, float testX, float testY, float playerWidth, float playerHeight,
                                         const std::vector<ecs::entity_t> &platforms, bool movingDown, bool isDownPressed = false);
        std::size_t registry_index_for_client(uint32_t clientId);
        bool finalize_player_death(uint32_t clientId, std::size_t registryIndex);
        void reset_player_motion(uint32_t clientId);
        void handle_jump_request(uint32_t clientId, bool onGround, float &velocity,
                                 PlayerInputState &state, int &jumpCount);
        void integrate_vertical_motion(uint32_t clientId, float dt, std::pair<float, float> &pos,
                                       float playerWidth, float playerHeight, float &velocity,
                                       PlayerInputState &state);
        template <typename Component>
        static Component *component_ptr(ecs::registry &registry, ecs::entity_t entity) {
            auto &arr = registry.get_components<Component>();
            auto idx = static_cast<std::size_t>(entity);
            if (idx >= arr.size() || !arr[idx].has_value())
                return nullptr;
            return &arr[idx].value();
        }

        struct PendingPacket {
            std::vector<uint8_t> data;
            asio::ip::udp::endpoint endpoint;
        };

        std::mutex packetMutex;
        std::queue<PendingPacket> pendingPackets;
        std::mutex initialClientsMutex;
        std::map<uint32_t, bool> initialClients;

        void broadcast_states_to_clients();
        void broadcast_player_death(uint32_t clientId);
        void broadcast_player_health();
        void broadcast_global_score();
        void broadcast_individual_scores();
        void broadcast_game_winner(uint32_t winnerId);

        void broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float z, float w, float h, float d);
        void broadcast_obstacle_despawn(uint32_t obstacleId);

        void sleep_to_maintain_tick(const std::chrono::high_resolution_clock::time_point &start, int tick_ms);
};
