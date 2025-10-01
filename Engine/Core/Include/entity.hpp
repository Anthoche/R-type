/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** Entity identifier class
*/

#pragma once

#include <cstddef>
#include <utility>

namespace ecs {

/**
 * @class entity_t
 * @brief Represents an entity within the ECS through a unique numeric identifier.
 *
 * This class encapsulates an entity ID and provides conversions
 * and comparisons for use in an Entity Component System (ECS).
 * It ensures type safety compared to using raw size_t values directly.
 */
class entity_t {
    public:
        /**
         * @brief Construct an entity identifier from a numeric value.
         * @param id The numeric identifier to assign to the entity.
         */
        explicit entity_t(std::size_t id) noexcept;

        /**
         * @brief Implicit conversion operator to retrieve the identifier.
         * @return The entity's identifier as a std::size_t.
         */
        operator std::size_t() const noexcept;

        /**
         * @brief Explicit accessor for the entity's identifier.
         * @return The stored entity identifier.
         */
        std::size_t value() const noexcept;

        /**
         * @brief Equality operator between two entities.
         * @param lhs Left-hand side entity.
         * @param rhs Right-hand side entity.
         * @return True if both entities share the same identifier.
         */
        friend bool operator==(const entity_t &lhs, const entity_t &rhs) noexcept;

        /**
         * @brief Inequality operator between two entities.
         * @param lhs Left-hand side entity.
         * @param rhs Right-hand side entity.
         * @return True if both entities have different identifiers.
         */
        friend bool operator!=(const entity_t &lhs, const entity_t &rhs) noexcept;

    private:
        std::size_t _id; ///< Internal unique entity identifier.
};

} // namespace ecs
