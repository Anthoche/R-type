/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_array.cpp
*/

#include <criterion/criterion.h>
#include "sparse_array.hpp"

using namespace ecs;

// Simple component type
struct Position {
    int x;
    int y;
};

// -------------------------
// Construction & size
// -------------------------
Test(SparseArray, default_constructed_is_empty) {
    sparse_array<int> arr;

    cr_assert_eq(arr.size(), 0, "default sparse_array should have size 0");
    cr_assert(arr.begin() == arr.end(), "begin and end should match in empty container");
}

// -------------------------
// operator[] (mutable + const)
// -------------------------
Test(SparseArray, operator_brackets_resizes) {
    sparse_array<int> arr;

    arr[5] = 42; // should auto-resize to index 5
    cr_assert_eq(arr.size(), 6, "index 5 should resize array to 6");
    cr_assert(arr[5].has_value(), "index 5 should hold a value");
    cr_assert_eq(arr[5].value(), 42, "should be assigned correctly");
}

Test(SparseArray, operator_brackets_const_out_of_bounds) {
    const sparse_array<int> arr;
        auto &slot = arr[10];

    cr_assert(!slot.has_value(), "out-of-bounds const access should return empty optional");
}

// -------------------------
// insert_at
// -------------------------
Test(SparseArray, insert_at_copy_and_move) {
    sparse_array<Position> arr;

    Position p1{1, 2};
    arr.insert_at(0, p1);
    cr_assert(arr[0].has_value());
    cr_assert_eq(arr[0]->x, 1);
    cr_assert_eq(arr[0]->y, 2);

    arr.insert_at(1, Position{3, 4});
    cr_assert(arr[1].has_value());
    cr_assert_eq(arr[1]->x, 3);
    cr_assert_eq(arr[1]->y, 4);
}

// -------------------------
// emplace_at
// -------------------------
Test(SparseArray, emplace_at_constructs_in_place) {
    sparse_array<Position> arr;

    arr.emplace_at(2, 9, 8);
    cr_assert(arr[2].has_value());
    cr_assert_eq(arr[2]->x, 9);
    cr_assert_eq(arr[2]->y, 8);
    cr_assert_eq(arr.size(), 3, "array should resize to at least index+1");
}

// -------------------------
// erase
// -------------------------
Test(SparseArray, erase_resets_slot) {
    sparse_array<int> arr;
    arr.insert_at(0, 123);

    arr.erase(0);
    cr_assert(!arr[0].has_value(), "erased slot should become empty");
}

Test(SparseArray, erase_out_of_bounds_safe) {
    sparse_array<int> arr;
    arr.insert_at(0, 1);

    arr.erase(99);
    cr_assert(arr[0].has_value(), "erasing out-of-bounds should not affect valid slots");
}

// -------------------------
// get_index
// -------------------------
Test(SparseArray, get_index_returns_correct_position) {
    sparse_array<int> arr;
    arr.insert_at(0, 42);
    arr.insert_at(1, 84);

    auto &slot = arr[1];
    auto idx = arr.get_index(slot);

    cr_assert_eq(idx, 1, "get_index should return correct index of slot");
}

Test(SparseArray, get_index_returns_invalid) {
    sparse_array<int> arr1;
    sparse_array<int> arr2;
    arr1.insert_at(0, 5);
    arr2.insert_at(0, 7);

    auto &foreign_slot = arr2[0];
    auto idx = arr1.get_index(foreign_slot);

    cr_assert_eq(idx, static_cast<size_t>(-1),
        "get_index on foreign slot should return -1");
}

// -------------------------
// Iteration
// -------------------------
Test(SparseArray, iterators_work) {
    sparse_array<int> arr;
    arr.insert_at(0, 10);
    arr.insert_at(1, 20);
    arr.insert_at(2, 30);

    int sum = 0;
    for (auto &slot : arr) {
        if (slot) sum += slot.value();
    }

    cr_assert_eq(sum, 60, "iteration should visit all elements in order");
}
