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
#include "../../Engine/Core/Entities/Include/player.hpp"
#include "../../Engine/Core/Entities/Include/enemy.hpp"
#include "../../Engine/Core/Entities/Include/obstacle.hpp"
#include "../../Engine/Core/Entities/Include/hitbox.hpp"
#include "../../Engine/Core/Entities/Include/background.hpp"
#include "../../Engine/Core/Entities/Include/sound.hpp"
#include "../../Engine/Core/Entities/Include/text.hpp"
#include "../../Engine/Core/Entities/Include/random_element.hpp"
#include "../../Physics/Include/Collision.hpp"
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

        // ==================== LIFECYCLE METHODS ====================
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

        // ==================== EVENT HANDLING ====================
        /**
         * @brief Handle player input.
         * @param input_x Horizontal input (movement).
         * @param input_y Vertical input (movement).
         */
        void handle_input(float input_x, float input_y);

        // ==================== ACCESSORS ====================
        /**
         * @brief Get the ECS registry.
         * @return Reference to the current registry.
         */
        ecs::registry &get_registry() { return _registry; }

        /**
         * @brief Get the player entity.
         * @return The current player entity.
         */
        ecs::entity_t get_player() const { return _player; }

        /**
         * @brief Get the obstacles list.
         * @return Reference to the current obstacles.
         */
        const std::vector<ecs::entity_t> &get_obstacles() const { return _obstacles; }

    private:
        // ==================== INITIALIZATION ====================
        /**
         * @brief Register all ECS components.
         */
        void register_components();

        /**
         * @brief Setup all game systems.
         */
        void setup_systems();

        // ==================== GAME LOOP ====================
        /**
         * @brief Update the game state (called every frame).
         */
        void update();

        /**
         * @brief Synchronize network players with ECS.
         */
        void sync_network_players();

        /**
         * @brief Remove disconnected players from the scene.
         * @param netPlayers Map of network players.
         */
        void remove_disconnected_players(const std::unordered_map<uint32_t, std::pair<float, float>> &netPlayers);

        /**
         * @brief Update player positions from network.
         * @param netPlayers Map of network players.
         */
        void update_player_positions(const std::unordered_map<uint32_t, std::pair<float, float>> &netPlayers);

        /**
         * @brief Save current positions to previous_position component.
         */
        void save_previous_positions();

        // ==================== RENDERING ====================
        /**
         * @brief Render all entities in the registry.
         */
        void render_all_entities();

        /**
         * @brief Render an entity with a type component.
         * @param i Entity index.
         * @param entity Entity handle.
         * @param x X position.
         * @param y Y position.
         * @param type Entity type.
         * @param drawables Drawable components array.
         * @param sprites Sprite components array.
         * @param texts Text components array.
         */
        void render_typed_entity(std::size_t i, ecs::entity_t entity, float x, float y,
                                 component::entity_type type,
                                 ecs::sparse_array<component::drawable> &drawables,
                                 ecs::sparse_array<component::sprite> &sprites,
                                 ecs::sparse_array<component::text> &texts);

        /**
         * @brief Render an entity without a type component.
         * @param i Entity index.
         * @param x X position.
         * @param y Y position.
         * @param drawables Drawable components array.
         * @param sprites Sprite components array.
         */
        void render_untyped_entity(std::size_t i, float x, float y,
                                   ecs::sparse_array<component::drawable> &drawables,
                                   ecs::sparse_array<component::sprite> &sprites);

        // ==================== RENDER BY TYPE ====================
        /**
         * @brief Render a text entity.
         */
        void render_text(std::size_t i, float x, float y, ecs::sparse_array<component::text> &texts);

        /**
         * @brief Render a player entity.
         */
        void render_player(std::size_t i, ecs::entity_t entity, float x, float y,
                          ecs::sparse_array<component::drawable> &drawables,
                          ecs::sparse_array<component::sprite> &sprites);

        /**
         * @brief Render an enemy entity.
         */
        void render_enemy(std::size_t i, float x, float y,
                         ecs::sparse_array<component::drawable> &drawables,
                         ecs::sparse_array<component::sprite> &sprites);

        /**
         * @brief Render an obstacle entity.
         */
        void render_obstacle(std::size_t i, float x, float y,
                            ecs::sparse_array<component::drawable> &drawables,
                            ecs::sparse_array<component::sprite> &sprites);

        /**
         * @brief Render a random element entity.
         */
        void render_random_element(std::size_t i, float x, float y,
                                   ecs::sparse_array<component::drawable> &drawables,
                                   ecs::sparse_array<component::sprite> &sprites);

        /**
         * @brief Render a background entity.
         */
        void render_background(std::size_t i, float x, float y,
                              ecs::sparse_array<component::drawable> &drawables,
                              ecs::sparse_array<component::sprite> &sprites);

        /**
         * @brief Render an entity with unknown type.
         */
        void render_default(std::size_t i, float x, float y,
                           ecs::sparse_array<component::drawable> &drawables);

        // ==================== RENDER PRIMITIVES ====================
        /**
         * @brief Render a sprite at given position.
         */
        void render_sprite(float x, float y, float width, float height,
                          const Texture2D &texture, Color tint);

        /**
         * @brief Render a rectangle at given position.
         */
        void render_rectangle(float x, float y, float width, float height, Color color);

        // ==================== UTILITIES ====================
        /**
         * @brief Get the network ID of a player entity.
         * @param entity Player entity.
         * @return Network player ID.
         */
        uint32_t get_player_network_id(ecs::entity_t entity) const;

        /**
         * @brief Get a color for a player ID.
         * @param id Player network ID.
         * @return Color for this player.
         */
        Color get_color_for_player_id(uint32_t id) const;

        // ==================== SYSTEMS ====================
        /**
         * @brief Setup the movement system for entities.
         */
        void setup_movement_system();

        /**
         * @brief Setup the rendering system for entities.
         */
        void setup_render_system();

        /**
         * @brief Setup the health system for entities.
         */
        void setup_health_system();

        /**
         * @brief Setup the enemy AI system.
         */
        void setup_enemy_ai_system();

        // ==================== ENTITY CREATION ====================
        /**
         * @brief Create the player entity (currently unused).
         */
        void create_player();

        /**
         * @brief Create obstacles (currently unused).
         */
        void create_obstacles();

        /**
         * @brief Create enemies (currently unused).
         */
        void create_enemies();

        // ==================== COLLISION ====================
        /**
         * @brief Check and resolve collisions for all entities.
         */
        void check_collisions();

        // ==================== MEMBER VARIABLES ====================
        ecs::entity_t _player; ///< Local player entity.
        std::vector<ecs::entity_t> _obstacles; ///< List of active obstacle entities.
        std::vector<ecs::entity_t> _enemys; ///< List of active enemy entities.
        std::unordered_map<uint32_t, ecs::entity_t> _playerEntities; ///< Map: network player ID -> ECS entity.

        bool _game_running; ///< Indicates whether the game is running.
        double _startTime; ///< Start time of the scene.
        Game &_game; ///< Reference to the game instance.
    };
} // namespace game::scene

#endif // GAMESCENE_HPP