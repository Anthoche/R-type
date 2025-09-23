/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_registry.cpp
*/

#include <criterion/criterion.h>
#include "registry.hpp"

Test(Registry, spawn_and_kill_entity) {
    ecs::registry reg;

    ecs::entity_t e1 = reg.spawn_entity();
    ecs::entity_t e2 = reg.spawn_entity();
    cr_assert_neq(e1, e2, "id must be unique");

    reg.kill_entity(e1);

    ecs::entity_t e3 = reg.spawn_entity();
    cr_assert_eq((std::size_t)e1, (
        std::size_t)e3,
        "killed id should be reused when spawning new entity");
}
