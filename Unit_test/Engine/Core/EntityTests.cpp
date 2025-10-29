/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_entity.cpp
*/

#include <algorithm>
#include <cstdint>
#include <gtest/gtest.h>
#include <unordered_set>
#include "entity.hpp"

using namespace ecs;

TEST(entity, create_and_compare) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    EXPECT_EQ(e1, e2);
    EXPECT_NE(e1, e3);
}

TEST(Entity, construct_value) {
    entity_t e1(42);

    EXPECT_EQ(e1.value(), 42);
}

TEST(Entity, conversion_operator) {
    entity_t e1(99);

    std::size_t id = e1;
    EXPECT_EQ(id, 99);
}

TEST(Entity, equality_operator) {
    entity_t e1(1);
    entity_t e2(1);
    entity_t e3(2);

    EXPECT_TRUE(e1 == e2);
    EXPECT_FALSE(e1 != e2);
    EXPECT_TRUE(e1 != e3);
    EXPECT_FALSE(e1 == e3);
}

TEST(Entity, copy_assign) {
    entity_t e1(123);
    entity_t e2 = e1;
    entity_t e3(999);
    e3 = e1; 

    EXPECT_EQ(e2.value(), 123);
    EXPECT_EQ(e3.value(), 123);
}

TEST(Entity, const_usage) {
    const entity_t e1(555);
    std::size_t id1 = e1.value();
    std::size_t id2 = e1;

    EXPECT_EQ(id1, 555);
    EXPECT_EQ(id2, 555);
}

TEST(Entity, move_construct_and_assign) {
    entity_t e1(42);
    entity_t e2(std::move(e1));

    EXPECT_EQ(e2.value(), 42);

    entity_t e3(99);
    e3 = std::move(e2);
    EXPECT_EQ(e3.value(), 42);
}

TEST(Entity, ordering_with_std_sort) {
    std::vector<entity_t> entities = { entity_t(5), entity_t(1), entity_t(3) };
    std::sort(entities.begin(), entities.end(), [](const entity_t &a, const entity_t &b){
        return a.value() < b.value();
    });

    EXPECT_EQ(entities[0].value(), 1);
    EXPECT_EQ(entities[1].value(), 3);
    EXPECT_EQ(entities[2].value(), 5);
}

TEST(Entity, boundary_values) {
    entity_t e_min(0);
    entity_t e_max(std::numeric_limits<std::size_t>::max());

    EXPECT_EQ(e_min.value(), 0);
    EXPECT_EQ(e_max.value(), std::numeric_limits<std::size_t>::max());
}

TEST(Entity, vector_of_entities_unique) {
    std::vector<entity_t> entities;
    for (std::size_t i = 0; i < 100; ++i) {
        entities.emplace_back(i);
    }

    std::unordered_set<std::size_t> ids;
    for (auto &e : entities) {
        ids.insert(e.value());
    }

    EXPECT_EQ(ids.size(), 100);
}

TEST(Entity, swap_entities) {
    entity_t e1(1), e2(2);
    std::swap(e1, e2);

    EXPECT_EQ(e1.value(), 2);
    EXPECT_EQ(e2.value(), 1);
}

TEST(Entity, comparison_edge_cases) {
    entity_t e1(0), e2(0), e3(1);

    EXPECT_TRUE(e1 == e2);
    EXPECT_FALSE(e1 != e2);
    EXPECT_TRUE(e1 != e3);
    EXPECT_FALSE(e1 == e3);
}
