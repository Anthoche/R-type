/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** projectile
*/

#include "Include/projectile.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {
    
ecs::entity_t create_projectile(ecs::registry &reg, float x, float y, 
                                  float vx, float vy, uint32_t ownerId) {
    auto projectile = reg.spawn_entity();
    
    reg.emplace_component<component::position>(projectile, x, y);
    reg.emplace_component<component::velocity>(projectile, vx, vy);
    reg.emplace_component<component::type>(projectile, component::entity_type::PROJECTILE);
    reg.emplace_component<component::damage>(projectile, 10);
    
    component::drawable drawable;
    drawable.width = 10.f;
    drawable.height = 5.f;
    drawable.color = WHITE;
    reg.add_component<component::drawable>(projectile, std::move(drawable));    
    reg.emplace_component<component::collision_box>(projectile, 10.f, 5.f);
    
    return projectile;
}

} // namespace game::entities