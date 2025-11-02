/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** TriggerZone
*/

#include "Include/triggerzone.hpp"
#include "Include/hitbox.hpp"

namespace game::entities {

    ecs::entity_t create_triggerzone(ecs::registry &reg, float x, float y, float z,
        float width, float height, float depth, const std::string &id)
    {
        auto zone = reg.spawn_entity();
        reg.emplace_component<component::position>(zone, x, y, z);
        reg.emplace_component<component::previous_position>(zone, x, y, z);
        reg.emplace_component<component::velocity>(zone, 0.f, 0.f, 0.f);
        reg.emplace_component<component::type>(zone, component::entity_type::TRIGGERZONE);
        reg.emplace_component<component::collision_box>(zone, width, height, depth);

        component::clickable trigger;
        trigger.id = id;
        reg.emplace_component<component::clickable>(zone, std::move(trigger));

        create_hitbox_for(reg, zone);
        return zone;
    }

}
