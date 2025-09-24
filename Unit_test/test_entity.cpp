/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_entity.cpp
*/

#include <criterion/criterion.h>
#include "entity.hpp"

using namespace ecs;

Test(entity, create_and_compare) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    cr_assert_eq(e1, e2, "should be equal");
    cr_assert_neq(e1, e3, "should not be equal");
}

// -------------------------
// Construction & value()
// -------------------------
Test(Entity, construct_and_value) {
    entity_t e1(42);

    cr_assert_eq(e1.value(), 42,
        "Entity value() should return the stored ID");
}

// -------------------------
// Conversion operator
// -------------------------
Test(Entity, conversion_operator) {
    entity_t e1(99);

    std::size_t id = e1;
    cr_assert_eq(id, 99,
        "Implicit conversion should return the entity's ID");
}

// -------------------------
// Equality / Inequality
// -------------------------
Test(Entity, equality_operator) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    cr_assert(e1 == e2,
        "Entities with the same ID should be equal");
    cr_assert(!(e1 != e2),
        "Entities with the same ID should not be unequal");

    cr_assert(e1 != e3,
        "Entities with different IDs should not be equal");
    cr_assert(!(e1 == e3),
        "Entities with different IDs should not compare as equal");
}

// -------------------------
// Copy semantics
// -------------------------
Test(Entity, copy_and_assign) {
    entity_t e1(123);
    entity_t e2 = e1;
    entity_t e3(999);

    e3 = e1; 

    cr_assert_eq(e2.value(), 123,
        "Copied entity should preserve ID");
    cr_assert_eq(e3.value(), 123,
        "Assigned entity should preserve ID");
}

// -------------------------
// Const correctness
// -------------------------
Test(Entity, const_usage) {
    const entity_t e1(555);

    std::size_t id1 = e1.value();
    std::size_t id2 = e1;

    cr_assert_eq(id1, 555, "Const value() should return correct ID");
    cr_assert_eq(id2, 555, "Const conversion should return correct ID");
}
