/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_entity.cpp
*/

#include <criterion/criterion.h>
#include "entity.hpp"

using namespace ecs;

// -------------------------
// Comparison
// -------------------------
Test(entity, create_and_compare) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    cr_assert_eq(e1, e2, "should be  equal");
    cr_assert_neq(e1, e3, "should not be  equal");
}

// -------------------------
// Construction & value()
// -------------------------
Test(Entity, construct_value) {
    entity_t e1(42);

    cr_assert_eq(e1.value(), 42,
        "entity value() should return the stored ID");
}

// -------------------------
// Conversion operator
// -------------------------
Test(Entity, conversion_operator) {
    entity_t e1(99);

    std::size_t id = e1;
    cr_assert_eq(id, 99,
        "conversion should return the entity's id");
}

// -------------------------
// Equality / Inequality
// -------------------------
Test(Entity, equality_operator) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    cr_assert(e1 == e2,
        "entities with the same id should be equal");
    cr_assert(!(e1 != e2),
        "entities with the same id should not be unequal");
    cr_assert(e1 != e3,
        "entities with different ids should not be equal");
    cr_assert(!(e1 == e3),
        "entities with different ids should not compare as equal");
}

// -------------------------
// Copy semantics
// -------------------------
Test(Entity, copy_assign) {
    entity_t e1(123);
    entity_t e2 = e1;
    entity_t e3(999);
    e3 = e1; 

    cr_assert_eq(e2.value(), 123,
        "copied entity should preserve id");
    cr_assert_eq(e3.value(), 123,
        "assigned entity should preserve id");
}

// -------------------------
// Const correctness
// -------------------------
Test(Entity, const_usage) {
    const entity_t e1(555);
    std::size_t id1 = e1.value();
    std::size_t id2 = e1;

    cr_assert_eq(id1, 555, "const value() should return correct id");
    cr_assert_eq(id2, 555, "const conversion should return correct id");
}
