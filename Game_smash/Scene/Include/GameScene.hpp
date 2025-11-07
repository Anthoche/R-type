/**
 * @file GameScene.hpp
 * @brief Main game scene managing gameplay, rendering, and player interactions
 * 
 * EPITECH PROJECT, 2025
 * rtype
 */

#pragma once

#include "../../Engine/Rendering/scene/Include/AScene.hpp"
#include "../Game.hpp"
#include "../../Shared/protocol.hpp"
#include <unordered_map>
#include <tuple>
#include "UI.hpp"

namespace game::scene {
    /**
     * @class GameScene
     * @brief Core game scene handling gameplay loop, rendering, and network synchronization
     * 
     * Manages the main game state including player entities, obstacles, platforms,
     * animations, and network player synchronization. Handles input processing,
     * visual state updates, and game-over conditions.
     */
    class GameScene : public AScene {
    public:
        Game &_game; ///< Reference to the main game instance

        /**
         * @brief Constructs the game scene
         * @param game Reference to the Game instance
         */
        GameScene(Game &game);

        /**
         * @brief Default destructor
         */
        ~GameScene() override = default;

        /**
         * @brief Initializes the scene, loading resources and setting up systems
         */
        void init() override;

        /**
         * @brief Renders all game entities and UI elements
         */
        void render() override;

        /**
         * @brief Processes input events and user interactions
         */
        void handleEvents() override;

        /**
         * @brief Cleanup method called when the scene closes
         */
        void onClose() override;

        /**
         * @brief Handles player input for movement and actions
         * @param input_x Horizontal input value
         * @param input_y Vertical input value
         * @param downPressed Whether the down key is pressed
         */
        void handle_input(float input_x, float input_y, bool downPressed);

        /**
         * @brief Gets the ECS registry
         * @return Reference to the entity component system registry
         */
        ecs::registry &get_registry() { return _registry; }

        /**
         * @brief Gets the player entity
         * @return The player entity ID
         */
        ecs::entity_t get_player() const { return _player; }

        /**
         * @brief Gets all obstacle entities
         * @return Vector of obstacle entity IDs
         */
        const std::vector<ecs::entity_t> &get_obstacles() const { return _obstacles; }
        
        /**
         * @brief Gets all platform entities
         * @return Vector of platform entity IDs
         */
        const std::vector<ecs::entity_t> &get_platforms() const { return _platforms; }

        /**
         * @brief Gets the game instance
         * @return Reference to the Game object
         */
        Game &getGame() { return _game; }

    private:
        /**
         * @struct InputState
         * @brief Stores the current state of all input keys
         */
        struct InputState {
            bool up{false};     ///< Up key pressed
            bool down{false};   ///< Down key pressed
            bool left{false};   ///< Left key pressed
            bool right{false};  ///< Right key pressed
            bool j{false};      ///< J key pressed (action 1)
            bool k{false};      ///< K key pressed (action 2)
        };

        /**
         * @enum PlayerAnimState
         * @brief Animation states for player character
         */
        enum class PlayerAnimState {
            Idle,   ///< Standing still
            Attack, ///< Attacking animation
            Hit,    ///< Taking damage animation
            Jump,   ///< Jumping animation
            Fall    ///< Falling animation
        };

        /**
         * @enum PlayerFacing
         * @brief Direction the player is facing
         */
        enum class PlayerFacing {
            Right, ///< Facing right
            Left   ///< Facing left
        };

        /**
         * @struct PlayerSpriteFrames
         * @brief Sprite sheet rectangles for each animation state
         */
        struct PlayerSpriteFrames {
            Rectangle idle{};   ///< Idle animation frame
            Rectangle attack{}; ///< Attack animation frame
            Rectangle hit{};    ///< Hit animation frame
            Rectangle jump{};   ///< Jump animation frame
            Rectangle fall{};   ///< Fall animation frame
        };

        /**
         * @struct PlayerVisualState
         * @brief Tracks visual state and animation timing for a player
         */
        struct PlayerVisualState {
            PlayerAnimState state{PlayerAnimState::Idle}; ///< Current animation state
            PlayerFacing facing{PlayerFacing::Right};     ///< Current facing direction
            float stateTimer{0.f};                        ///< Time in current state
            float attackTimer{0.f};                       ///< Attack animation timer
            float hitTimer{0.f};                          ///< Hit animation timer
            Vector2 previousPosition{0.f, 0.f};           ///< Last known position
            bool hasPrevious{false};                      ///< Whether previous position is valid
            float verticalVelocity{0.f};                  ///< Vertical movement speed
            int lastKnownHealth{-1};                      ///< Last health value
            bool airborne{false};                         ///< Whether player is in the air
            float airborneTimer{0.f};                     ///< Time spent airborne
        };

        /**
         * @brief Updates game logic and systems
         */
        void update();

        /**
         * @brief Configures the movement system for entities
         */
        void setup_movement_system();

        /**
         * @brief Configures the rendering system
         */
        void setup_render_system();

        /**
         * @brief Configures the health management system
         */
        void setup_health_system();

        /**
         * @brief Loads all entity textures from disk
         */
        void load_entity_textures();

        /**
         * @brief Unloads all entity textures from memory
         */
        void unload_entity_textures();

        /**
         * @brief Retrieves the texture for a specific entity
         * @param entity The entity ID
         * @return Pointer to the entity's texture, or nullptr if not found
         */
        Texture2D* get_entity_texture(ecs::entity_t entity);

        /**
         * @brief Toggles fullscreen mode
         */
        void toggleFullScreen();

        ecs::entity_t _player;                          ///< Local player entity
        Music _music;                                   ///< Background music
        Sound _shootSound;                              ///< Shooting sound effect
        Sound _victorySound;                            ///< Victory sound effect
        Sound _defeatSound;                             ///< Defeat sound effect
        bool _victorySoundPlayed = false;               ///< Whether victory sound has played
        bool _defeatSoundPlayed = false;                ///< Whether defeat sound has played
        std::vector<ecs::entity_t> _obstacles;          ///< All obstacle entities
        std::vector<ecs::entity_t> _platforms;          ///< All platform entities
        std::vector<ecs::entity_t> _decorations;        ///< All decoration entities
        std::unordered_map<uint32_t, ecs::entity_t> _playerEntities; ///< Network player entities mapped by client ID
        bool _isDead = false;                           ///< Whether the player is dead
        bool _isWin = false;                            ///< Whether the player has won
        std::unordered_map<uint32_t, Texture2D> _entityTextures; ///< Cached entity textures
        std::unordered_map<uint32_t, float> moovePlayer; ///< Player movement data

        bool _game_running;                             ///< Whether the game is currently running
        double _startTime;                              ///< Game start timestamp
        UI _ui;                                         ///< User interface manager

        /**
         * @brief Indexes all existing entities in the scene
         */
        void index_existing_entities();

        /**
         * @brief Renders all entities in the scene
         */
        void render_entities();
        
        /**
         * @brief Renders a player entity
         * @param entity The player entity
         * @param pos Position component
         * @param draw Drawable component
         */
        void render_player(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        
        /**
         * @brief Renders an obstacle entity
         * @param entity The obstacle entity
         * @param pos Position component
         * @param draw Drawable component
         */
        void render_obstacle(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        
        /**
         * @brief Renders a background entity
         * @param entity The background entity
         * @param pos Position component
         * @param draw Drawable component
         */
        void render_background(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        
        /**
         * @brief Renders a powerup entity
         * @param entity The powerup entity
         * @param pos Position component
         * @param draw Drawable component
         */
        void render_powerup(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        
        /**
         * @brief Renders a platform entity
         * @param entity The platform entity
         * @param pos Position component
         * @param draw Drawable component
         */
        void render_platform(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        
        /**
         * @brief Renders a decoration entity
         * @param entity The decoration entity
         * @param pos Position component
         * @param draw Drawable component
         */
        void render_decoration(ecs::entity_t entity, const component::position &pos, const component::drawable &draw);
        
        /**
         * @brief Loads and plays background music
         */
        void load_music();

        /**
         * @brief Renders obstacles received from network
         */
        void render_network_obstacles();
        
        /**
         * @brief Renders the death screen overlay
         */
        void render_death_screen();
        
        /**
         * @brief Renders the victory screen overlay
         */
        void render_win_screen();

        /**
         * @brief Generates a unique color based on player ID
         * @param id Player client ID
         * @return Color for the player
         */
        Color get_color_for_id(uint32_t id);
        
        /**
         * @brief Renders the player portrait in the UI
         */
        void render_player_portrait();
        
        /**
         * @brief Initializes sprite frame rectangles for animations
         */
        void initialize_player_sprite_frames();
        
        /**
         * @brief Gets the source rectangle for a player animation
         * @param state The animation state
         * @param facing The facing direction
         * @return Rectangle defining the sprite sheet region
         */
        const Rectangle &get_player_source_rect(PlayerAnimState state, PlayerFacing facing) const;
        
        /**
         * @brief Updates visual states for all network players
         * @param netPlayers Map of player positions and data
         * @param netHealth Map of player health values
         * @param dt Delta time since last update
         */
        void update_player_visual_states(
            const std::unordered_map<uint32_t, std::tuple<float, float, float>> &netPlayers,
            const std::unordered_map<uint32_t, std::pair<int16_t, int16_t>> &netHealth,
            float dt);
        
        /**
         * @brief Applies attack state for local player
         * @param clientId Client ID of the player
         * @param triggerAttack Whether to trigger attack animation
         * @param faceLeft Whether player is facing left
         * @param faceRight Whether player is facing right
         */
        void apply_local_attack_state(uint32_t clientId, bool triggerAttack, bool faceLeft, bool faceRight);
        
        /**
         * @brief Dispatches input events to game systems
         * @param upPressed Up key state
         * @param downPressed Down key state
         * @param leftPressed Left key state
         * @param rightPressed Right key state
         * @param jPressed J key state
         * @param kPressed K key state
         */
        void dispatch_input_events(bool upPressed, bool downPressed, bool leftPressed, bool rightPressed, bool jPressed, bool kPressed);
        
        InputState _inputState;                         ///< Current input state
        PlayerSpriteFrames _rightFrames{};              ///< Sprite frames for right-facing animations
        PlayerSpriteFrames _leftFrames{};               ///< Sprite frames for left-facing animations
        std::unordered_map<uint32_t, PlayerVisualState> _playerVisualStates; ///< Visual states for all players
    };
} // namespace game::scene