/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameScene
*/

#pragma once

#include <unordered_map>
#include <nlohmann/json_fwd.hpp>
#include "UI.hpp"
#include "../Game.hpp"
#include "ChatSystem.hpp"
#include "../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../../Engine/Core/Entities/Include/components.hpp"
#include "../../Shared/protocol.hpp"
#include "../../Shared/WeaponDefinition.hpp"

namespace game::scene {
    /**
     * @class GameScene
     * @brief Represents the main game scene for R-Type.
     *
     * This scene manages the game loop logic, including:
     * - Initialization and rendering.
     * - Handling player input and events.
     * - Managing entities (player, enemies, obstacles).
     * - Setting up ECS systems (movement, rendering, collision, health).
     * - Collision detection and game state updates.
     */
    class GameScene : public AScene {
    public:
        Game &_game; ///< Reference to the game instance.

        /**
         * @brief Construct a GameScene with a reference to the Game instance.
         * @param game Reference to the game instance.
         */
        GameScene(Game &game);

        /**
         * @brief Default destructor.
         */
        ~GameScene() override = default;

        // --- Overridden lifecycle methods ---
        /**
         * @brief Initialize the scene with entities, systems, and state.
         */
        void init() override;

        /**
         * @brief Render the scene, including all entities and components.
         */
        void render() override;

        /**
         * @brief Handle input and system events.
         */
        void handleEvents() override;

        /**
         * @brief Perform cleanup when the scene is closed.
         */
        void onClose() override;

        // --- Event handling ---
        /**
         * @brief Handle shooting action from the player.
         * @param weaponDef Weapon definition for the shot.
         * @param cooldown Time interval between shots.
         */
        void handle_shoot(const weapon::WeaponDefinition &weaponDef, float cooldown);

        /**
         * @brief Handle movement input from the player.
         * @param input_x Horizontal input (-1.0 to 1.0).
         * @param input_y Vertical input (-1.0 to 1.0).
         */
        void handle_input(float input_x, float input_y);

        // --- Accessors ---
        /**
         * @brief Get the ECS registry.
         * @return Reference to the ECS registry.
         */
        ecs::registry &get_registry() { return _registry; }

        /**
         * @brief Get the local player entity.
         * @return Player entity ID.
         */
        ecs::entity_t get_player() const { return _player; }

        /**
         * @brief Get the current obstacles in the scene.
         * @return Vector of obstacle entity IDs.
         */
        const std::vector<ecs::entity_t> &get_obstacles() const { return _obstacles; }

        /**
         * @brief Get the main Game instance.
         * @return Reference to the Game object.
         */
        Game &getGame() { return _game; }

    private:
        enum class EndGameState {
            None,
            Victory,
            Defeat
        };

        struct InputState {
            bool up{false};    ///< Is up key pressed
            bool down{false};  ///< Is down key pressed
            bool left{false};  ///< Is left key pressed
            bool right{false}; ///< Is right key pressed
        } _inputState;

        // --- Game logic ---
        /**
         * @brief Update the game state; called every frame.
         */
        void update();

        // --- Game systems ---
        /**
         * @brief Setup movement system for entities.
         */
        void setup_movement_system();

        /**
         * @brief Setup rendering system for entities.
         */
        void setup_render_system();

        /**
         * @brief Setup custom collision detection system.
         */
        void setup_collision_system();

        /**
         * @brief Setup engine-defined ECS collision system.
         */
        void setup_ecs_collision_system();

        /**
         * @brief Setup AI system for enemy entities.
         */
        void setup_enemy_ai_system();

        /**
         * @brief Setup health system for all entities.
         */
        void setup_health_system();

        // --- Entity creation ---
        /**
         * @brief Create enemy entities that move horizontally.
         */
        void create_enemies();

        /**
         * @brief Create the player entity and initialize its components.
         */
        void create_player();

        /**
         * @brief Create obstacle entities in the scene.
         */
        void create_obstacles();

        // --- Collision management ---
        /**
         * @brief Check and resolve collisions for all entities.
         */
        void check_collisions();

        /**
         * @brief Check collisions of projectiles on enemies.
         */
        void check_projectile_enemy_collisions();

        /**
         * @brief Synchronize hitboxes immediately with their parent entities.
         */
        void sync_hitboxes_immediate();

        // --- Texture management ---
        /**
         * @brief Load textures for entities that have sprite components.
         */
        void load_entity_textures();

        /**
         * @brief Unload all loaded entity textures.
         */
        void unload_entity_textures();

        /**
         * @brief Load textures for projectile entities.
         */
        void load_projectile_textures();

        /**
         * @brief Unload all projectile textures.
         */
        void unload_projectile_textures();

        /**
         * @brief Get the texture associated with a given entity.
         * @param entity Entity to get texture for.
         * @return Pointer to Texture2D or nullptr if not found.
         */
        Texture2D* get_entity_texture(ecs::entity_t entity);

        /**
         * @brief Process a full ECS registry received from the network.
         * @return True if processing succeeded.
         */
        bool processPendingFullRegistry();

        /**
         * @brief Clear all level entities for reloading the level.
         */
        void clearLevelEntitiesForReload();

        /**
         * @brief Remove entities of a specific type from the scene.
         * @param type Type of entity to remove.
         */
        void removeEntitiesOfType(component::entity_type type);

        /**
         * @brief Build sprite maps from a JSON representation of the registry.
         * @param registryJson JSON object representing the registry.
         */
        void buildSpriteMapsFromRegistry(const nlohmann::json &registryJson);

        /**
         * @brief Toggle fullscreen mode for the game window.
         */
        void toggleFullScreen();

        // --- Rendering helpers ---
        /**
         * @brief Synchronize ECS state with network messages.
         */
        void sync_network_state();

        /**
         * @brief Draw ECS entities in layer order.
         */
        void draw_ecs_layers();

        /**
         * @brief Extract enemy sprite paths from registry.
         */
        void extract_enemy_sprite_paths();

        /**
         * @brief Index existing entities in the ECS registry for fast access.
         */
        void index_existing_entities();

        /**
         * @brief Render all entities in the scene.
         */
        void render_entities();

        /**
         * @brief Render a single player entity.
         */
        void render_player(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);

        /**
         * @brief Render a single enemy entity.
         */
        void render_enemy(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);

        /**
         * @brief Render a single obstacle entity.
         */
        void render_obstacle(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);

        /**
         * @brief Render a background entity.
         */
        void render_background(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);

        /**
         * @brief Render a text component.
         */
        void render_text(ecs::entity_t entity, const component::position &pos);

        /**
         * @brief Render a powerup entity.
         */
        void render_powerup(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);

        /**
         * @brief Render a projectile entity.
         */
        void render_projectile(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);

        /**
         * @brief Load and initialize background music.
         */
        void load_music();

        /**
         * @brief Render obstacles received from the network.
         */
        void render_network_obstacles();

        /**
         * @brief Render enemies received from the network.
         */
        void render_network_enemies();

        /**
         * @brief Render projectiles received from the network.
         */
        void render_network_projectiles();

        /**
         * @brief Render enemy projectiles received from the network.
         */
        void render_network_enemy_projectiles();

        /**
         * @brief Render the death screen when the player dies.
         */
        void render_death_screen();

        /**
         * @brief Render the victory screen when the player wins.
         */
        void render_win_screen();

        /**
         * @brief Render the final victory screen.
         */
        void render_final_win_screen();

        /**
         * @brief Get a unique color associated with a client ID.
         * @param id Client ID.
         * @return Color associated with the client.
         */
        Color get_color_for_id(uint32_t id);

        /**
         * @brief Return true if no player entities remain alive on the server snapshot.
         */
        bool areAllPlayersDead();

        /**
         * @brief Schedule the transition towards the stats scene.
         */
        void scheduleStatsScene(EndGameState state);

        /**
         * @brief Update the state machine that decides when to show the stats scene.
         */
        void updateStatsSceneTransition();

        /**
         * @brief Open the stats scene once the delay has elapsed.
         */
        void handleStatsSceneLaunch();

        /**
         * @brief Dispatch input events to the InputState struct.
         */
        void dispatch_input_events(bool upPressed, bool downPressed, bool leftPressed, bool rightPressed);

        // --- Game state ---
        ecs::entity_t _player; ///< Local player entity.
        Music _music; ///< Background music instance.
        Sound _shootSound; ///< Sound effect for shooting.
        Sound _victorySound; ///< Sound effect for victory.
        Sound _defeatSound; ///< Sound effect for defeat.
        bool _victorySoundPlayed = false; ///< Flag to track if victory sound has been played.
        bool _defeatSoundPlayed = false; ///< Flag to track if defeat sound has been played.
        std::vector<ecs::entity_t> _obstacles; ///< List of active obstacle entities.
        std::unordered_map<uint32_t, ecs::entity_t> _obstacleMap;  // Map serverId -> client entity
        std::unordered_map<uint32_t, std::string> _obstacleSpriteMap;  // Map serverId -> sprite path
        std::vector<ecs::entity_t> _enemys; ///< List of active enemy entities.
        std::vector<ecs::entity_t> _elements; ///< List of active random element entities.
        std::unordered_map<uint32_t, ecs::entity_t> _elementMap; ///< Map: network element ID -> ECS entity.
        std::unordered_map<uint32_t, std::string> _elementSpriteMap;  // Map serverId -> sprite path
        std::unordered_map<uint32_t, ecs::entity_t> _enemyMap; ///< Map: network enemy ID -> ECS entity.
        std::unordered_map<uint32_t, std::string> _enemySpriteMap; ///< Map: network enemy ID -> sprite path.
        std::unordered_map<uint32_t, ecs::entity_t> _playerEntities; ///< Map: network player ID -> ECS entity.
        bool _isDead = false; ///< Flag indicating if the local player is dead.
        bool _isWin = false; ///< Flag indicating if the local player has won.
        std::unordered_map<uint32_t, Texture2D> _entityTextures; ///< Map: entity ID -> loaded texture.
        std::unordered_map<std::string, Texture2D> _projectileTextures; ///< Map: projectile type -> loaded texture.
        std::unordered_map<uint32_t, float> moovePlayer; ///< Map: client ID -> movement offset for sprite rendering.
        float _backgroundScrollX = 0.0f; ///< Background scrolling offset.
        float _victoryStartTime = 0.0f;
        float _stopShoot = false;
        bool _hasLevelData = false;
        bool _levelReloadPending = false;
        struct WeaponUsageState {
            float lastShotTime{0.f};
            float burstStartTime{0.f};
            float lastBurstEndTime{-1.f};
            int remainingAmmo{-1};
            bool inBurst{false};
        };
        std::unordered_map<std::string, WeaponUsageState> _weaponUsage;
        bool _lastBoss = false;

        // --- Game state ---
        bool _game_running; ///< Indicates whether the game is running.
        double _startTime; ///< Start time of the scene.
        UI _ui; ///< UI instance for game overlay
        ChatSystem _chat; ///< Chat overlay manager
        bool _statsSceneScheduled{false};
        double _statsSceneStart{0.0};
        const double _statsSceneDelay{8.0};
        EndGameState _endGameState{EndGameState::None};
        bool _openStatsRequested{false};
};

} // namespace game::scene
