/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_zipper.cpp
*/

#include <cstdint>
#include <gtest/gtest.h>
#include "sparse_array.hpp"
#include "zipper.hpp"

using namespace ecs;
using namespace ecs::containers;

struct Position { 
    int x; 
    int y; 
};

struct Velocity { 
    int dx; 
    int dy; 
};

TEST(Zipper, dereference_returns_tuple) {
    sparse_array<Position> positions;
    sparse_array<Velocity> velocities;

    positions.insert_at(0, Position{10, 20});
    velocities.insert_at(0, Velocity{30, 40});

    zipper zip(positions, velocities);
    auto it = zip.begin();

    auto [pOpt, vOpt] = *it;
    EXPECT_TRUE(pOpt.has_value());
    EXPECT_EQ(pOpt->x, 10);
    EXPECT_TRUE(vOpt.has_value());
    EXPECT_EQ(vOpt->dy, 40);
}

TEST(Zipper, equality_operators) {
    sparse_array<Position> positions;
    sparse_array<Velocity> velocities;

    positions.insert_at(0, Position{1, 1});
    velocities.insert_at(0, Velocity{2, 2});

    zipper zip(positions, velocities);

    auto it1 = zip.begin();
    auto it2 = zip.begin();
    auto itEnd = zip.end();

    EXPECT_TRUE(it1 == it2);
    EXPECT_TRUE(it1 != itEnd);
}

TEST(Zipper, empty_containers) {
    sparse_array<Position> positions;
    sparse_array<Velocity> velocities;

    zipper zip(positions, velocities);
    auto begin = zip.begin();
    auto end = zip.end();

    EXPECT_TRUE(begin == end);
}