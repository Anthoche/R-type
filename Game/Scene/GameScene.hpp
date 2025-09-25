/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameScene
*/
#ifndef GAMESCENE_HPP
#define GAMESCENE_HPP

#include "../../Engine/Rendering/scene/AScene.hpp"
#include "../../Engine/Core/Include/entity.hpp"
#include "../Entities/Include/player.hpp"
#include "../Entities/Include/obstacle.hpp"
#include "../Entities/Include/hitbox.hpp"
#include "../Entities/Include/background.hpp"
#include "../Entities/Include/sound.hpp"
#include "../Entities/Include/text.hpp"
#include "../Entities/Include/random_element.hpp"
#include "../Game.hpp"
#include "../Système/Collision.hpp"
#include <unordered_map>

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

    private:
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
         * @brief Synchronize hitboxes with their parent entity immediately.
         */
        void sync_hitboxes_immediate();

        // --- Entities ---
        ecs::entity_t _player; ///< Local player entity.
        std::vector<ecs::entity_t> _obstacles; ///< List of active obstacle entities.
        std::unordered_map<uint32_t, ecs::entity_t> _playerEntities; ///< Map: network player ID -> ECS entity.

        // --- Game state ---
        bool _game_running; ///< Indicates whether the game is running.
        double _startTime; ///< Start time of the scene.
        Game &_game; ///< Reference to the game instance.

        // --- Helpers ---
        /**
         * @brief Synchronize ECS state with network messages.
         */
        void sync_network_state();

        /**
         * @brief Render ECS entities organized by layer.
         */
        void draw_ecs_layers();
    };
} // namespace game::scene

#endif // GAMESCENE_HPP
