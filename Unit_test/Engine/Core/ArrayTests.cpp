/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** test_array.cpp
*/

#include <cstdint>
#include <gtest/gtest.h>
#include "sparse_array.hpp"

using namespace ecs;

struct Position {
    int x;
    int y;
};

TEST(SparseArray, default_empty) {
    sparse_array<int> arr;

    EXPECT_EQ(arr.size(), 0);
    EXPECT_TRUE(arr.begin() == arr.end());
}

TEST(SparseArray, operator_brackets_resizes) {
    sparse_array<int> arr;
    arr[5] = 42;
    
    EXPECT_EQ(arr.size(), 6);
    EXPECT_TRUE(arr[5].has_value());
    EXPECT_EQ(arr[5].value(), 42);
}

TEST(SparseArray, const_out_of_bounds) {
    const sparse_array<int> arr;
        auto &slot = arr[10];

    EXPECT_FALSE(slot.has_value());
}

TEST(SparseArray, insert_move) {
    sparse_array<Position> arr;

    Position p1{1, 2};
    arr.insert_at(0, p1);
    EXPECT_TRUE(arr[0].has_value());
    EXPECT_EQ(arr[0]->x, 1);
    EXPECT_EQ(arr[0]->y, 2);

    arr.insert_at(1, Position{3, 4});
    EXPECT_TRUE(arr[1].has_value());
    EXPECT_EQ(arr[1]->x, 3);
    EXPECT_EQ(arr[1]->y, 4);
}

TEST(SparseArray, emplace_at) {
    sparse_array<Position> arr;

    arr.emplace_at(2, 9, 8);
    EXPECT_TRUE(arr[2].has_value());
    EXPECT_EQ(arr[2]->x, 9);
    EXPECT_EQ(arr[2]->y, 8);
    EXPECT_EQ(arr.size(), 3);
}

TEST(SparseArray, erase_resets_slot) {
    sparse_array<int> arr;
    arr.insert_at(0, 123);

    arr.erase(0);
    EXPECT_FALSE(arr[0].has_value());
}

TEST(SparseArray, erase_out_of_bounds) {
    sparse_array<int> arr;
    arr.insert_at(0, 1);

    arr.erase(99);
    EXPECT_TRUE(arr[0].has_value());
}

TEST(SparseArray, get_index_correct) {
    sparse_array<int> arr;
    arr.insert_at(0, 42);
    arr.insert_at(1, 84);

    auto &slot = arr[1];
    auto idx = arr.get_index(slot);

    EXPECT_EQ(idx, 1);
}

TEST(SparseArray, get_index_invalid) {
    sparse_array<int> arr1;
    sparse_array<int> arr2;
    arr1.insert_at(0, 5);
    arr2.insert_at(0, 7);

    auto &foreign_slot = arr2[0];
    auto idx = arr1.get_index(foreign_slot);

    EXPECT_EQ(idx, static_cast<size_t>(-1));
}

TEST(SparseArray, iterators_work) {
    sparse_array<int> arr;
    arr.insert_at(0, 10);
    arr.insert_at(1, 20);
    arr.insert_at(2, 30);

    int sum = 0;
    for (auto &slot : arr) {
        if (slot) sum += slot.value();
    }

    EXPECT_EQ(sum, 60);
}
