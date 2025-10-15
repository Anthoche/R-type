/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_entity.cpp
*/

#include <cstdint>
#include <gtest/gtest.h>
#include "entity.hpp"

using namespace ecs;

// -------------------------
// Comparison
// -------------------------
TEST(entity, create_and_compare) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    EXPECT_EQ(e1, e2);
    EXPECT_NE(e1, e3);
}

// -------------------------
// Construction & value()
// -------------------------
TEST(Entity, construct_value) {
    entity_t e1(42);

    EXPECT_EQ(e1.value(), 42);
}

// -------------------------
// Conversion operator
// -------------------------
TEST(Entity, conversion_operator) {
    entity_t e1(99);

    std::size_t id = e1;
    EXPECT_EQ(id, 99);
}

// -------------------------
// Equality / Inequality
// -------------------------
TEST(Entity, equality_operator) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    EXPECT_TRUE(e1 == e2);
    EXPECT_FALSE(e1 != e2);
    EXPECT_TRUE(e1 != e3);
    EXPECT_FALSE(e1 == e3);
}

// -------------------------
// Copy semantics
// -------------------------
TEST(Entity, copy_assign) {
    entity_t e1(123);
    entity_t e2 = e1;
    entity_t e3(999);
    e3 = e1; 

    EXPECT_EQ(e2.value(), 123);
    EXPECT_EQ(e3.value(), 123);
}

// -------------------------
// Const correctness
// -------------------------
TEST(Entity, const_usage) {
    const entity_t e1(555);
    std::size_t id1 = e1.value();
    std::size_t id2 = e1;

    EXPECT_EQ(id1, 555);
    EXPECT_EQ(id2, 555);
}
