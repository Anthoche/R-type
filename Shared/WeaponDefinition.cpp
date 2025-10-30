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
                .spreadAngle = 0.f,
                .burstDuration = 0.f,
                .burstCooldown = 0.f
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
                .spreadAngle = 0.f,
                .burstDuration = 0.f,
                .burstCooldown = 0.f
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
                .spreadAngle = 15.f,
                .burstDuration = 0.f,
                .burstCooldown = 0.f
            };
            defs.emplace(spread.id, spread);

            constexpr float MINIGUN_SHOT_INTERVAL = 0.08f;
            constexpr float MINIGUN_BURST_DURATION = 5.f;
            constexpr int MINIGUN_MAG_SIZE = static_cast<int>(MINIGUN_BURST_DURATION / MINIGUN_SHOT_INTERVAL);
            WeaponDefinition minigun{
                .id = "minigun",
                .displayName = "Vulcan Minigun",
                .description = "Empties a drum over 5 seconds, then vents for 2 seconds.",
                .infiniteAmmo = false,
                .ammoCapacity = MINIGUN_MAG_SIZE > 0 ? MINIGUN_MAG_SIZE : 1,
                .fireCooldown = MINIGUN_SHOT_INTERVAL,
                .minCooldown = MINIGUN_SHOT_INTERVAL,
                .damage = 0.6f,
                .projectileSpeed = 420.f,
                .projectileLifetime = 4.f,
                .projectileWidth = 9.f,
                .projectileHeight = 6.f,
                .spreadAngle = 4.f,
                .burstDuration = MINIGUN_BURST_DURATION,
                .burstCooldown = 2.f
            };
            defs.emplace(minigun.id, minigun);

            WeaponDefinition rpg{
                .id = "rpg",
                .displayName = "Titan RPG",
                .description = "Slow firing rocket with massive explosive punch.",
                .infiniteAmmo = true,
                .ammoCapacity = 0,
                .fireCooldown = 2.4f,
                .minCooldown = 1.6f,
                .damage = 12.f,
                .projectileSpeed = 280.f,
                .projectileLifetime = 6.5f,
                .projectileWidth = 24.f,
                .projectileHeight = 12.f,
                .spreadAngle = 0.f,
                .burstDuration = 0.f,
                .burstCooldown = 0.f
            };
            defs.emplace(rpg.id, rpg);

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
