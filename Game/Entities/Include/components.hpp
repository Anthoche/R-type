/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** component
*/

#pragma once

/**
 * @namespace component
 * @brief Contains all ECS components used in the R-Type game.
 */
namespace component {

    /**
     * @struct position
     * @brief Represents the position of an entity in 2D space.
     */
    struct position {
        float x{0.f}; ///< X coordinate
        float y{0.f}; ///< Y coordinate
    };

    /**
     * @struct velocity
     * @brief Represents the velocity of an entity in 2D space.
     */
    struct velocity {
        float vx{0.f}; ///< Velocity along X
        float vy{0.f}; ///< Velocity along Y
    };

    /**
     * @struct drawable
     * @brief Represents a renderable entity with size and color.
     */
    struct drawable {
        float width{32.f};  ///< Width of the entity
        float height{32.f}; ///< Height of the entity
        float r{1.f}, g{1.f}, b{1.f}, a{1.f}; ///< Color in RGBA
    };

    /**
     * @struct controllable
     * @brief Marks an entity as player-controllable.
     */
    struct controllable {
        float speed{200.f}; ///< Movement speed
    };

    /**
     * @struct health
     * @brief Represents health points of an entity.
     */
    struct health {
        int current{100}; ///< Current health
        int max{100};     ///< Maximum health
    };

    /**
     * @struct damage
     * @brief Represents damage dealt by an entity.
     */
    struct damage {
        int amount{10}; ///< Damage amount
    };

    /**
     * @enum entity_type
     * @brief Defines the type of an entity for game logic.
     */
    enum class entity_type {
        PLAYER,    ///< Player entity
        ENEMY,     ///< Enemy entity
        BULLET,    ///< Projectile
        POWERUP,   ///< Power-up item
        OBSTACLE   ///< Obstacle in the game
    };

    /**
     * @struct type
     * @brief Component to store the entity type.
     */
    struct type {
        entity_type value{entity_type::PLAYER}; ///< Type of the entity
    };

    /**
     * @struct collision_box
     * @brief Defines the collision bounds of an entity.
     */
    struct collision_box {
        float width{32.f};  ///< Width of the collision box
        float height{32.f}; ///< Height of the collision box
    };
}
