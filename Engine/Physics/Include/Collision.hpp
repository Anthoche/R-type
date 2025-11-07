/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Collision helper functions
*/

#pragma once
#include "../../Core/Include/registry.hpp"
#include "../../Core/Entities/Include/components.hpp"
#include "../../Core/Include/entity.hpp"
#include <vector>

namespace physics::collision {

    /**
     * @brief Find the hitbox entity associated with a given owner entity.
     * @param registry ECS registry containing the hitbox links.
     * @param owner Owner entity to search for.
     * @return Entity ID of the hitbox, or 0 if not found.
     */
    ecs::entity_t find_hitbox_of(ecs::registry &registry, ecs::entity_t owner);

    /**
     * @brief Check if movement to a test position would be blocked by an obstacle.
     * @param registry ECS registry containing obstacle components.
     * @param obstacles Collection of obstacle entities to test against.
     * @param testX Target X position.
     * @param testY Target Y position.
     * @param box Collision box of the moving entity.
     * @return True if blocked, false otherwise.
     */
    bool is_blocked(ecs::registry &registry,
                    const std::vector<ecs::entity_t> &obstacles,
                    float testX,
                    float testY,
                    const component::collision_box &box);

    /**
     * @brief Resolve a collision between an entity and an obstacle.
     *        Applies position correction and updates velocity/previous position.
     * @param registry ECS registry containing the entity components.
     * @param entity The colliding entity.
     * @param entityPos Reference to the entity's position.
     * @param entityBox Reference to the entity's collision box.
     * @param obsPos Reference to the obstacle's position.
     * @param obsBox Reference to the obstacle's collision box.
     * @param prevX Previous X position.
     * @param prevY Previous Y position.
     */
    void resolve_collision(ecs::registry &registry,
                           ecs::entity_t entity,
                           component::position &entityPos,
                           component::collision_box &entityBox,
                           const component::position &obsPos,
                           const component::collision_box &obsBox,
                           float prevX,
                           float prevY);

    /**
     * @brief Handle collisions for a specific entity against a set of obstacles.
     * @param registry ECS registry containing all components.
     * @param obstacles Collection of obstacle entities to test against.
     * @param entity ECS entity to test and resolve collisions for.
     */
    void handle_entity_collisions(ecs::registry &registry,
                                  const std::vector<ecs::entity_t> &obstacles,
                                  ecs::entity_t entity);

        /**
         * @brief Check overlap between two AABBs (Axis-Aligned Bounding Boxes).
         * @return True if A and B overlap, false otherwise.
         */
        bool overlap_aabb(float leftA, float rightA, float topA, float bottomA,
                        float leftB, float rightB, float topB, float bottomB);

} // namespace physics::collision
