/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameScene
*/


#pragma once

#include "../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../Game.hpp"
#include "../../Shared/protocol.hpp"
#include "../../Engine/Core/Entities/Include/components.hpp"
#include <unordered_map>
#include "UI.hpp"
#include <nlohmann/json_fwd.hpp>

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
         * @brief Initialize the scene (entities, systems, state).
         */
        void init() override;

        /**
         * @brief Render the scene (draw entities/components).
         */
        void render() override;

        /**
         * @brief Handle input and system events.
         */
        void handleEvents() override;

        /**
         * @brief Called when the scene is closed.
         */
        void onClose() override;

        // --- Event handling ---

        /**
         * @brief handle shoot input.
         * @param cooldown Cooldown for shoot
         */
        void handle_shoot(float cooldown);

        /**
         * @brief Handle player input.
         * @param input_x Horizontal input (movement).
         * @param input_y Vertical input (movement).
         */
        void handle_input(float input_x, float input_y);

        // --- Accessors ---
        /**
         * @brief getter to send registry
         * @return return the current registry.
         */
        ecs::registry &get_registry() { return _registry; }

        /**
         * @brief getter to send player
         * @return return the current player.
         */
        ecs::entity_t get_player() const { return _player; }

        /**
         * @brief getter to send obstacles
         * @return return tje current obstacles.
         */
        const std::vector<ecs::entity_t> &get_obstacles() const { return _obstacles; }

        /**
         * @brief Provides access to the main Game instance.
         * @return A reference to the current Game object.
         */
        Game &getGame() { return _game; }

    private:
        struct InputState {
            bool up{false};
            bool down{false};
            bool left{false};
            bool right{false};
        };

        // --- Game logic ---
        /**
         * @brief Update the game state (called every frame).
         */
        void update();

        // --- Game systems ---
        /**
         * @brief Setup the movement system for entities.
         */
        void setup_movement_system();

        /**
         * @brief Setup the rendering system for entities.
         */
        void setup_render_system();

        /**
         * @brief Setup the collision detection system (custom).
         */
        void setup_collision_system();

        /**
         * @brief Setup the ECS collision system (engine-defined).
         */
        void setup_ecs_collision_system();

        /**
         * @brief Setup the enemy AI system for entities.
         */
        void setup_enemy_ai_system();

        /**
         * @brief Setup the health system for entities.
         */
        void setup_health_system();

        // --- Entity creation ---
        /**
         * @brief Create enemy entities that move horizontally.
         */
        void create_enemies();

        /**
         * @brief Create the player entity and initialize components.
         */
        void create_player();

        /**
         * @brief Create obstacles in the scene.
         */
        void create_obstacles();

        // --- Collision management ---
        /**
         * @brief Check and resolve collisions for all entities.
         */
        void check_collisions();

        /**
         * @brief Check and resolve collisions for projectile on enemy.
         */
        void check_projectile_enemy_collisions();

        /**
         * @brief Synchronize hitboxes with their parent entity immediately.
         */
        void sync_hitboxes_immediate();

        // --- Texture management ---
        /**
         * @brief Load all textures for entities that have a sprite component.
         * Creates a mapping between entity IDs and their loaded textures.
         */
        void load_entity_textures();

        /**
         * @brief Unload all loaded textures.
         */
        void unload_entity_textures();

        /**
         * @brief Load all textures for projectiles.
         */
        void load_projectile_textures();

        /**
         * @brief Unload all loaded projectile textures.
         */
        void unload_projectile_textures();

        /**
         * @brief Get the texture associated with an entity.
         * @param entity The entity to get the texture for.
         * @return Pointer to the texture if found, nullptr otherwise.
         */
        Texture2D* get_entity_texture(ecs::entity_t entity);

        
        bool processPendingFullRegistry();
        void clearLevelEntitiesForReload();
        void removeEntitiesOfType(component::entity_type type);
        void buildSpriteMapsFromRegistry(const nlohmann::json &registryJson);
        
        void toggleFullScreen();

        // --- Entities ---
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
        bool _lastBoss = false;

        // --- Game state ---
        bool _game_running; ///< Indicates whether the game is running.
        double _startTime; ///< Start time of the scene.
        UI _ui; ///< UI instance for game overlay

        // --- Helpers ---
        /**
         * @brief Synchronize ECS state with network messages.
         */
        void sync_network_state();

        /**
         * @brief Render ECS entities organized by layer.
         */
        void draw_ecs_layers();

        // --- Indexation des entités ---
        void index_existing_entities();

        // --- Rendu générique ---
        void render_entities();
        void render_player(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_enemy(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_obstacle(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_background(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_text(ecs::entity_t entity, const component::position &pos);
        void render_powerup(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void render_projectile(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        void load_music();

        // --- Rendu des entités réseau ---
        void render_network_obstacles();
        void render_network_enemies();
        void render_network_projectiles();
        void render_network_enemy_projectiles();
        void render_death_screen();
        void render_win_screen();
        void render_final_win_screen();


        // --- Utilitaires ---
        Color get_color_for_id(uint32_t id);
        void dispatch_input_events(bool upPressed, bool downPressed, bool leftPressed, bool rightPressed);
        InputState _inputState;
    };
} // namespace game::scene
