/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Collision helper functions
*/

#pragma once
#include "../Engine/Core/Entities/Include/components.hpp"
#include "../Engine/Core/Include/entity.hpp"
#include <vector>

namespace game::scene {

    class GameScene;

    namespace collision {

    /**
     * @brief Find the hitbox entity associated with the local player.
     * @param scene Reference to the game scene.
     * @return Entity ID of the player's hitbox, or 0 if not found.
     */
    ecs::entity_t find_player_hitbox(GameScene &scene);

    /**
     * @brief Find the hitbox entity associated with a given owner entity.
     * @param scene Reference to the game scene.
     * @param owner Owner entity.
     * @return Entity ID of the hitbox, or 0 if not found.
     */
    ecs::entity_t find_hitbox_of(GameScene &scene, ecs::entity_t owner);

    /**
     * @brief Check if movement to a test position would be blocked by an obstacle.
     * @param scene Reference to the game scene.
     * @param testX Target X position.
     * @param testY Target Y position.
     * @param playerPos Current player position.
     * @param playerBox Player collision box.
     * @return True if blocked, false otherwise.
     */
    bool is_blocked(GameScene &scene, float testX, float testY,
                    const component::position &playerPos,
                    const component::collision_box &playerBox);

    /**
     * @brief Resolve a collision between an entity and an obstacle.
     *        Applies position correction and updates velocity/previous position.
     * @param scene Reference to the game scene.
     * @param playerEntity The colliding entity.
     * @param playerPos Reference to the entity's position.
     * @param playerBox Reference to the entity's collision box.
     * @param obsPos Reference to the obstacle's position.
     * @param obsBox Reference to the obstacle's collision box.
     * @param prevX Previous X position.
     * @param prevY Previous Y position.
     */
    void resolve_collision(GameScene &scene,
                        ecs::entity_t playerEntity,
                        component::position &playerPos,
                        component::collision_box &playerBox,
                        const component::position &obsPos,
                        const component::collision_box &obsBox,
                        float prevX, float prevY);

    /**
     * @brief Handle collisions for a specific entity.
     * @param scene Reference to the game scene.
     * @param entity ECS entity to test and resolve collisions for.
     */
    void handle_entity_collisions(GameScene &scene, ecs::entity_t entity);

    /**
     * @brief Check overlap between two AABBs (Axis-Aligned Bounding Boxes).
     * @return True if A and B overlap, false otherwise.
     */
    bool overlap_aabb(float leftA, float rightA, float topA, float bottomA,
                    float leftB, float rightB, float topB, float bottomB);

    } // namespace collision
} // namespace game::scene
