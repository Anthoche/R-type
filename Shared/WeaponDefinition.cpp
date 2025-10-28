/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** WeaponDefinition implementation
*/

#include "WeaponDefinition.hpp"
#include <mutex>

namespace weapon {

    namespace {
        const WeaponDefinition &fallback_definition() {
            static const WeaponDefinition fallback{
                .id = "basic_shot",
                .displayName = "Basic Shot",
                .description = "Standard rapid-fire blaster with unlimited energy cells.",
                .infiniteAmmo = true,
                .ammoCapacity = 0,
                .fireCooldown = 0.8f,
                .minCooldown = 0.1f,
                .damage = 1.f,
                .projectileSpeed = 400.f,
                .projectileLifetime = 4.f,
                .projectileWidth = 10.f,
                .projectileHeight = 5.f,
                .spreadAngle = 0.f
            };
            return fallback;
        }

        std::unordered_map<std::string, WeaponDefinition> build_definitions() {
            std::unordered_map<std::string, WeaponDefinition> defs;

            WeaponDefinition basic = fallback_definition();
            defs.emplace(basic.id, basic);

            WeaponDefinition charged{
                .id = "charged_shot",
                .displayName = "Charged Shot",
                .description = "High-impact bolt with limited capacitors.",
                .infiniteAmmo = false,
                .ammoCapacity = 8,
                .fireCooldown = 1.6f,
                .minCooldown = 0.6f,
                .damage = 5.f,
                .projectileSpeed = 320.f,
                .projectileLifetime = 5.f,
                .projectileWidth = 18.f,
                .projectileHeight = 8.f,
                .spreadAngle = 0.f
            };
            defs.emplace(charged.id, charged);

            WeaponDefinition spread{
                .id = "spread_shot",
                .displayName = "Spread Shot",
                .description = "Triple-beam spread ideal for close range coverage.",
                .infiniteAmmo = false,
                .ammoCapacity = 24,
                .fireCooldown = 1.0f,
                .minCooldown = 0.4f,
                .damage = 0.8f,
                .projectileSpeed = 360.f,
                .projectileLifetime = 3.5f,
                .projectileWidth = 8.f,
                .projectileHeight = 8.f,
                .spreadAngle = 15.f
            };
            defs.emplace(spread.id, spread);

            return defs;
        }

        const std::unordered_map<std::string, WeaponDefinition> &definitions() {
            static const auto defs = build_definitions();
            return defs;
        }
    } // namespace

    const WeaponDefinition &getDefinition(const std::string &id) {
        const auto &defs = definitions();
        auto it = defs.find(id);
        if (it != defs.end()) {
            return it->second;
        }
        return fallback_definition();
    }

    bool hasDefinition(const std::string &id) {
        const auto &defs = definitions();
        return defs.find(id) != defs.end();
    }

    const std::unordered_map<std::string, WeaponDefinition> &allDefinitions() {
        return definitions();
    }

} // namespace weapon
