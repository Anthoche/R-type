/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** component_registration
*/

#pragma once
#include "components.hpp"
#include "../Engine/Core/Include/registry.hpp"

inline void register_all_components(ecs::registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::previous_position>();
    reg.register_component<component::velocity>();
    reg.register_component<component::drawable>();
    reg.register_component<component::sprite>();
    reg.register_component<component::audio>();
    reg.register_component<component::text>();
    reg.register_component<component::font>();
    reg.register_component<component::controllable>();
    reg.register_component<component::health>();
    reg.register_component<component::damage>();
    reg.register_component<component::type>();
    reg.register_component<component::collision_box>();
    reg.register_component<component::hitbox_link>();
}

