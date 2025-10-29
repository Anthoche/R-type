/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WeaponDefinition
*/

#pragma once

#include <string>
#include <unordered_map>

namespace weapon {

    struct WeaponDefinition {
        std::string id;
        std::string displayName;
        std::string description;
        bool infiniteAmmo{true};
        int ammoCapacity{0};
        float fireCooldown{0.8f};
        float minCooldown{0.1f};
        float damage{1.f};
        float projectileSpeed{400.f};
        float projectileLifetime{4.f};
        float projectileWidth{10.f};
        float projectileHeight{5.f};
        float spreadAngle{0.f};
        float burstDuration{0.f};
        float burstCooldown{0.f};
    };

    /**
     * @brief Retrieve the weapon definition for the given identifier.
     * @param id Weapon identifier.
     * @return Reference to the matching weapon definition (fallbacks to basic_shot).
     */
    const WeaponDefinition &getDefinition(const std::string &id);

    /**
     * @brief Check whether a weapon definition exists for the given identifier.
     * @param id Weapon identifier.
     * @return True if a definition exists, false otherwise.
     */
    bool hasDefinition(const std::string &id);

    /**
     * @brief Access the map of all registered weapon definitions.
     * @return Map of weapon definitions keyed by identifier.
     */
    const std::unordered_map<std::string, WeaponDefinition> &allDefinitions();

} // namespace weapon
