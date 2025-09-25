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

        /**
         * @brief Setup the health system for entities.
         */
        void setup_health_system();

        // --- Entity creation ---
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
         * @brief Check and resolve collisions for a specific entity.
         * @param entity ECS entity to test and resolve collisions for.
         */
        void handle_entity_collisions(ecs::entity_t entity);

        /**
         * @brief Synchronize hitboxes with their parent entity immediately.
         */
        void sync_hitboxes_immediate();

        /**
         * @brief Find the hitbox entity associated with the local player.
         * @return Entity ID of the player's hitbox, or 0 if not found.
         */
        ecs::entity_t find_player_hitbox();

        /**
         * @brief Find the hitbox entity associated with a given owner entity.
         * @param owner Owner entity.
         * @return Entity ID of the hitbox, or 0 if not found.
         */
        ecs::entity_t find_hitbox_of(ecs::entity_t owner);

        /**
         * @brief Check if movement to a test position would be blocked by an obstacle.
         * @param testX Target X position.
         * @param testY Target Y position.
         * @param playerPos Current player position.
         * @param playerBox Player collision box.
         * @return True if blocked, false otherwise.
         */
        bool is_blocked(float testX, float testY,
                        const component::position &playerPos,
                        const component::collision_box &playerBox);

        /**
         * @brief Resolve a collision between an entity and an obstacle.
         *        Applies position correction and updates velocity/previous position.
         * @param playerEntity The colliding entity.
         * @param playerPos Reference to the entity's position.
         * @param playerBox Reference to the entity's collision box.
         * @param obsPos Reference to the obstacle's position.
         * @param obsBox Reference to the obstacle's collision box.
         * @param prevX Previous X position.
         * @param prevY Previous Y position.
         */
        void resolve_collision(ecs::entity_t playerEntity,
                               component::position &playerPos,
                               component::collision_box &playerBox,
                               const component::position &obsPos,
                               const component::collision_box &obsBox,
                               float prevX, float prevY);

        /**
         * @brief Check overlap between two AABBs (Axis-Aligned Bounding Boxes).
         * @param leftA   Left coordinate of A.
         * @param rightA  Right coordinate of A.
         * @param topA    Top coordinate of A.
         * @param bottomA Bottom coordinate of A.
         * @param leftB   Left coordinate of B.
         * @param rightB  Right coordinate of B.
         * @param topB    Top coordinate of B.
         * @param bottomB Bottom coordinate of B.
         * @return True if A and B overlap, false otherwise.
         */
        bool overlap_aabb(float leftA, float rightA, float topA, float bottomA,
                          float leftB, float rightB, float topB, float bottomB);

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
