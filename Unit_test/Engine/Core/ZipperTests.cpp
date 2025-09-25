/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_zipper.cpp
*/

#include <cstdint>
#include <criterion/criterion.h>
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

// -------------------------
// Dereferencing (*, ->)
// -------------------------
Test(Zipper, dereference_returns_tuple) {
    sparse_array<Position> positions;
    sparse_array<Velocity> velocities;

    positions.insert_at(0, Position{10, 20});
    velocities.insert_at(0, Velocity{30, 40});

    zipper zip(positions, velocities);
    auto it = zip.begin();

    auto [pOpt, vOpt] = *it;
    cr_assert(pOpt);
    cr_assert_eq(pOpt->x, 10);
    cr_assert(vOpt);
    cr_assert_eq(vOpt->dy, 40);
}

// -------------------------
// Equality / inequality
// -------------------------
Test(Zipper, equality_operators) {
    sparse_array<Position> positions;
    sparse_array<Velocity> velocities;

    positions.insert_at(0, Position{1, 1});
    velocities.insert_at(0, Velocity{2, 2});

    zipper zip(positions, velocities);

    auto it1 = zip.begin();
    auto it2 = zip.begin();
    auto itEnd = zip.end();

    cr_assert(it1 == it2, "two should be equal");
    cr_assert(it1 != itEnd, "begin and end should differ");
}
