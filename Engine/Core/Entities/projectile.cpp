/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** projectile
*/

#include "Include/projectile.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_projectile(ecs::registry &reg,float x, float y, float z,
        float vx, float vy, float vz, uint32_t ownerId) {
        auto projectile = reg.spawn_entity();

        reg.emplace_component<component::position>(projectile, x, y, z);
        reg.emplace_component<component::velocity>(projectile, vx, vy, vz);
        reg.emplace_component<component::type>(projectile, component::entity_type::PROJECTILE);
        reg.emplace_component<component::damage>(projectile, 10);

        component::drawable draw;
        draw.width = 10.f;
        draw.height = 5.f;
        draw.depth = 5.f;
        draw.color = YELLOW;
        reg.add_component<component::drawable>(projectile, std::move(draw));

        reg.emplace_component<component::collision_box>(projectile, 10.f, 5.f, 5.f);

        create_hitbox_for(reg, projectile);

        return projectile;
    }

}
