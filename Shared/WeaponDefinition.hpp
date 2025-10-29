/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WeaponDefinition
*/

#pragma once

#include <string>
#include <unordered_map>

/**
 * @namespace weapon
 * @brief Contains weapon system definitions and configurations.
 *
 * This namespace provides the weapon definition system for the R-Type game,
 * including weapon statistics, projectile properties, and firing mechanics.
 */
namespace weapon {

    /**
     * @struct WeaponDefinition
     * @brief Defines the properties and behavior of a weapon type.
     *
     * Contains all configuration parameters for a weapon, including damage,
     * fire rate, projectile characteristics, ammo capacity, and special effects
     * like spread patterns and burst fire.
     */
    struct WeaponDefinition {
        std::string id;              ///< Unique identifier for this weapon type.
        std::string displayName;     ///< Human-readable name displayed in the UI.
        std::string description;     ///< Descriptive text explaining the weapon's characteristics.
        bool infiniteAmmo{true};     ///< Whether this weapon has unlimited ammunition.
        int ammoCapacity{0};         ///< Maximum ammunition count (0 if infinite).
        float fireCooldown{0.8f};    ///< Time in seconds between shots.
        float minCooldown{0.1f};     ///< Minimum possible cooldown (for upgrades/powerups).
        float damage{1.f};           ///< Damage dealt per projectile hit.
        float projectileSpeed{400.f}; ///< Speed of projectiles in units per second.
        float projectileLifetime{4.f}; ///< Duration in seconds before projectiles despawn.
        float projectileWidth{10.f};  ///< Width of projectile collision box.
        float projectileHeight{5.f};  ///< Height of projectile collision box.
        float spreadAngle{0.f};      ///< Angular spread for multi-shot weapons (degrees).
        float burstDuration{0.f};    ///< Duration of burst fire mode (0 for single shot).
        float burstCooldown{0.f};    ///< Cooldown between bursts.
    };

    /**
     * @brief Retrieves the weapon definition for a given identifier.
     *
     * Looks up the weapon configuration by its unique ID string.
     * If the ID is not found, returns a fallback definition (typically "basic_shot").
     *
     * @param id Unique weapon identifier string.
     * @return Const reference to the matching WeaponDefinition.
     */
    const WeaponDefinition &getDefinition(const std::string &id);

    /**
     * @brief Checks if a weapon definition exists for the given identifier.
     *
     * Useful for validating weapon IDs before attempting to retrieve them.
     *
     * @param id Unique weapon identifier string.
     * @return true if a definition exists for this ID, false otherwise.
     */
    bool hasDefinition(const std::string &id);

    /**
     * @brief Accesses the complete registry of all weapon definitions.
     *
     * Provides read-only access to the entire weapon definition database,
     * useful for UI displays, weapon selection menus, or configuration tools.
     *
     * @return Const reference to the map of all weapon definitions keyed by identifier.
     */
    const std::unordered_map<std::string, WeaponDefinition> &allDefinitions();

} // namespace weapon