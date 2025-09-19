/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** Zipper container and iterators
*/

#pragma once

#include <tuple>
#include <optional>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace ecs::containers {

/**
 * @class zipper_iterator
 * @brief Iterator that traverses multiple containers in parallel.
 *
 * This iterator advances all underlying iterators simultaneously
 * and yields tuples of values only when all optionals are set.
 *
 * @tparam Containers Container types (e.g., sparse_array<Component>).
 */
template <class... Containers>
class zipper_iterator {
    template <class Container>
    using iterator_t = decltype(std::declval<Container &>().begin());

    template <class Container>
    using it_reference_t = typename std::iterator_traits<iterator_t<Container>>::reference;

public:
    using value_type = std::tuple<typename std::remove_reference_t<decltype((*std::declval<iterator_t<Containers>&>()))>...>; ///< Dereferenced tuple of optionals.
    using reference = value_type;          ///< Reference type (tuple).
    using pointer = void;                  ///< Pointer not supported.
    using difference_type = std::ptrdiff_t; ///< Difference type.
    using iterator_category = std::input_iterator_tag; ///< Input iterator category.
    using iterator_tuple = std::tuple<iterator_t<Containers>...>; ///< Tuple of underlying iterators.

    template <class... Cs>
    friend class zipper;

    /**
     * @brief Construct from a tuple of iterators and a maximum bound.
     * @param it_tuple Tuple of starting iterators.
     * @param max Maximum number of iterations allowed.
     */
    zipper_iterator(iterator_tuple const &it_tuple, size_t max)
        : _current(it_tuple), _max(max), _idx(0) {
        // Advance to first valid set
        if (_max > 0) {
            while (_idx < _max && !all_set(std::index_sequence_for<Containers...>{})) {
                incr_all(std::index_sequence_for<Containers...>{});
            }
        }
    }

    zipper_iterator(zipper_iterator const &z) = default;

    /**
     * @brief Increment operator (prefix).
     * @return Iterator advanced to next valid tuple.
     */
    zipper_iterator operator++() { incr_all(std::index_sequence_for<Containers...>{}); return *this; }

    /**
     * @brief Increment operator (postfix).
     * @return Iterator advanced to next valid tuple.
     */
    zipper_iterator &operator++(int) { incr_all(std::index_sequence_for<Containers...>{}); return *this; }

    /**
     * @brief Dereference operator.
     * @return Tuple of dereferenced values.
     */
    value_type operator*() { return to_value(std::index_sequence_for<Containers...>{}); }

    /**
     * @brief Arrow operator.
     * @return Tuple of dereferenced values.
     */
    value_type operator->() { return to_value(std::index_sequence_for<Containers...>{}); }

    friend bool operator==(zipper_iterator const &lhs, zipper_iterator const &rhs) {
        return lhs._current == rhs._current;
    }
    friend bool operator!=(zipper_iterator const &lhs, zipper_iterator const &rhs) { return !(lhs == rhs); }

private:
    /// Advance all iterators until a valid tuple is found.
    template <size_t... Is>
    void incr_all(std::index_sequence<Is...>) {
        (++std::get<Is>(_current), ...);
        ++_idx;
        while (_idx < _max && !all_set(std::index_sequence_for<Containers...>{})) {
            (++std::get<Is>(_current), ...);
            ++_idx;
        }
    }

    /// Check if all optionals are set at current positions.
    template <size_t... Is>
    bool all_set(std::index_sequence<Is...>) {
        bool ok = true;
        (void)std::initializer_list<int>{(ok = ok && static_cast<bool>(*std::get<Is>(_current)), 0)...};
        return ok;
    }

    /// Convert current iterators to a tuple of values.
    template <size_t... Is>
    value_type to_value(std::index_sequence<Is...>) {
        return value_type{(*std::get<Is>(_current))...};
    }

private:
    iterator_tuple _current; ///< Current positions of all iterators.
    size_t _max;             ///< Maximum number of iterations.
    size_t _idx;             ///< Current index.
};

/**
 * @class zipper
 * @brief Adapter container producing a zipper_iterator.
 *
 * Iterates over multiple containers in lockstep,
 * yielding tuples of values when all are present.
 *
 * @tparam Containers Container types (e.g., sparse_array<Component>).
 */
template <class... Containers>
class zipper {
public:
    using iterator = zipper_iterator<Containers...>; ///< Parallel iterator type.
    using iterator_tuple = typename iterator::iterator_tuple; ///< Tuple of iterators.

    /**
     * @brief Construct zipper from multiple containers.
     * @param cs Containers to zip together.
     */
    zipper(Containers &...cs)
        : _begin(std::make_tuple(cs.begin()...)),
          _end(_compute_end(cs...)),
          _size(_compute_size(cs...)) {}

    iterator begin() { iterator it(_begin, _size); return it; }
    iterator end() { return iterator(_end, _size); }

private:
    static size_t _compute_size(Containers &...containers) { return std::min({static_cast<size_t>(containers.size())...}); }
    static iterator_tuple _compute_end(Containers &...containers) { return std::make_tuple(containers.end()...); }

private:
    iterator_tuple _begin; ///< Starting iterators.
    iterator_tuple _end;   ///< Ending iterators.
    size_t _size;          ///< Minimum size across containers.
};

/**
 * @class indexed_zipper_iterator
 * @brief Iterator variant that yields indices alongside zipped values.
 *
 * This is useful when you also need the entity index.
 *
 * @tparam Containers Container types (e.g., sparse_array<Component>).
 */
template <class... Containers>
class indexed_zipper_iterator {
    using base_iter = zipper_iterator<Containers...>;
public:
    using value_type = std::tuple<size_t, typename std::remove_reference_t<decltype((*std::declval<decltype(std::declval<Containers &>().begin())&>()))>...>; ///< (index, tuple of values)
    using reference = value_type;
    using pointer = void;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

    indexed_zipper_iterator(base_iter it, base_iter end)
        : _it(it), _end(end), _index(0) {}

    indexed_zipper_iterator &operator++() { ++_it; ++_index; return *this; }
    indexed_zipper_iterator &operator++(int) { ++_it; ++_index; return *this; }

    value_type operator*() { return std::tuple_cat(std::make_tuple(_index), *_it); }

    friend bool operator==(indexed_zipper_iterator const &l, indexed_zipper_iterator const &r) { return l._it == r._it; }
    friend bool operator!=(indexed_zipper_iterator const &l, indexed_zipper_iterator const &r) { return !(l == r); }

private:
    base_iter _it;   ///< Underlying zipper iterator.
    base_iter _end;  ///< End iterator.
    size_t _index;   ///< Current index.
};

/**
 * @class indexed_zipper
 * @brief Adapter container producing an indexed_zipper_iterator.
 *
 * Combines zipper iteration with entity indices.
 *
 * @tparam Containers Container types.
 */
template <class... Containers>
class indexed_zipper {
public:
    using iterator = indexed_zipper_iterator<Containers...>; ///< Indexed iterator type.
    using base = zipper<Containers...>; ///< Underlying zipper type.

    indexed_zipper(Containers &...cs) : _base(cs...) {}

    iterator begin() { return iterator(_base.begin(), _base.end()); }
    iterator end() { return iterator(_base.end(), _base.end()); }

private:
    base _base; ///< Underlying zipper instance.
};

} // namespace ecs::containers
