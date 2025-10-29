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
#include <algorithm>
#include <initializer_list>

namespace ecs::containers {

    /**
     * @class zipper_iterator
     * @brief Iterator that traverses multiple containers simultaneously in parallel.
     *
     * This iterator advances all underlying container iterators together and yields tuples
     * of values only when all containers have valid (non-empty optional) values at the current position.
     * Positions where any container has an empty optional are automatically skipped.
     *
     * @tparam Containers Container types to iterate over (typically sparse_array<Component>).
     */
    template <class... Containers>
    class zipper_iterator {
        template <class Container>
        using iterator_t = decltype(std::declval<Container &>().begin());

        template <class Container>
        using it_reference_t = typename std::iterator_traits<iterator_t<Container>>::reference;

        public:
            using value_type = std::tuple<typename std::remove_reference_t<decltype((*std::declval<iterator_t<Containers>&>()))>...>; ///< Tuple type containing dereferenced values from all containers.
            using reference = value_type;          ///< Reference type (returns tuple by value).
            using pointer = void;                  ///< Pointer access not supported.
            using difference_type = std::ptrdiff_t; ///< Signed integer type for iterator arithmetic.
            using iterator_category = std::input_iterator_tag; ///< Iterator category (input iterator).
            using iterator_tuple = std::tuple<iterator_t<Containers>...>; ///< Tuple containing all underlying iterators.

            template <class... Cs>
            friend class zipper;

            /**
             * @brief Constructs a zipper iterator from a tuple of iterators.
             *
             * Automatically advances to the first position where all containers have valid values.
             *
             * @param it_tuple Tuple of starting iterators for each container.
             * @param max Maximum number of elements to iterate over (minimum size across all containers).
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
             * @brief Prefix increment operator.
             *
             * Advances all iterators and skips positions until all have valid values.
             *
             * @return Reference to this iterator after incrementing.
             */
            zipper_iterator operator++() { incr_all(std::index_sequence_for<Containers...>{}); return *this; }

            /**
             * @brief Postfix increment operator.
             *
             * Advances all iterators and skips positions until all have valid values.
             *
             * @return Reference to this iterator after incrementing.
             */
            zipper_iterator &operator++(int) { incr_all(std::index_sequence_for<Containers...>{}); return *this; }

            /**
             * @brief Dereference operator.
             *
             * @return Tuple containing the current values from all containers.
             */
            value_type operator*() { return to_value(std::index_sequence_for<Containers...>{}); }

            /**
             * @brief Arrow operator.
             *
             * @return Tuple containing the current values from all containers.
             */
            value_type operator->() { return to_value(std::index_sequence_for<Containers...>{}); }

            friend bool operator==(zipper_iterator const &lhs, zipper_iterator const &rhs) {
                return lhs._current == rhs._current;
            }
            friend bool operator!=(zipper_iterator const &lhs, zipper_iterator const &rhs) { return !(lhs == rhs); }

        private:
            /**
             * @brief Advances all iterators until a valid position is found.
             *
             * @tparam Is Index sequence for parameter pack expansion.
             */
            template <size_t... Is>
            void incr_all(std::index_sequence<Is...>) {
                (++std::get<Is>(_current), ...);
                ++_idx;
                while (_idx < _max && !all_set(std::index_sequence_for<Containers...>{})) {
                    (++std::get<Is>(_current), ...);
                    ++_idx;
                }
            }

            /**
             * @brief Checks if all containers have valid (non-empty) values at the current position.
             *
             * @tparam Is Index sequence for parameter pack expansion.
             * @return true if all optionals are set, false otherwise.
             */
            template <size_t... Is>
            bool all_set(std::index_sequence<Is...>) {
                bool ok = true;
                (void)std::initializer_list<int>{(ok = ok && static_cast<bool>(*std::get<Is>(_current)), 0)...};
                return ok;
            }

            /**
             * @brief Converts the current iterator positions to a tuple of values.
             *
             * @tparam Is Index sequence for parameter pack expansion.
             * @return Tuple containing dereferenced values from all containers.
             */
            template <size_t... Is>
            value_type to_value(std::index_sequence<Is...>) {
                return value_type{(*std::get<Is>(_current))...};
            }

        private:
            iterator_tuple _current; ///< Current positions of all underlying iterators.
            size_t _max;             ///< Maximum number of elements to iterate.
            size_t _idx;             ///< Current iteration index.
    };

    
    /**
     * @class zipper
     * @brief Container adapter that enables parallel iteration over multiple containers.
     *
     * The zipper combines multiple containers (typically sparse arrays of components)
     * and provides iterators that yield tuples of values only at positions where
     * all containers have valid entries. This is essential for ECS systems that
     * operate on entities with specific component combinations.
     *
     * @tparam Containers Container types to zip together.
     */
    template <class... Containers>
    class zipper {
        public:
            using iterator = zipper_iterator<Containers...>; ///< Iterator type for parallel iteration.
            using iterator_tuple = typename iterator::iterator_tuple; ///< Tuple of underlying iterators.

            /**
             * @brief Constructs a zipper from multiple containers.
             *
             * @param cs References to the containers to iterate over in parallel.
             */
            zipper(Containers &...cs)
                : _begin(std::make_tuple(cs.begin()...)),
                _end(_compute_end(cs...)),
                _size(_compute_size(cs...)) {}

            /**
             * @brief Returns an iterator to the beginning.
             *
             * @return Iterator pointing to the first valid tuple of values.
             */
            iterator begin() { iterator it(_begin, _size); return it; }

            /**
             * @brief Returns an iterator to the end.
             *
             * @return Iterator representing the end of iteration.
             */
            iterator end() { return iterator(_end, _size); }

        private:
            /**
             * @brief Computes the minimum size across all containers.
             */
            static size_t _compute_size(Containers &...containers) { return std::min({static_cast<size_t>(containers.size())...}); }

            /**
             * @brief Creates a tuple of end iterators from all containers.
             */
            static iterator_tuple _compute_end(Containers &...containers) { return std::make_tuple(containers.end()...); }

        private:
            iterator_tuple _begin; ///< Tuple of begin iterators for all containers.
            iterator_tuple _end;   ///< Tuple of end iterators for all containers.
            size_t _size;          ///< Minimum size across all containers.
    };


    /**
     * @class indexed_zipper_iterator
     * @brief Iterator that yields both entity indices and component values.
     *
     * This iterator extends the zipper_iterator by including the entity index
     * in the returned tuple, making it useful when systems need to know which
     * entity they are processing.
     *
     * @tparam Containers Container types to iterate over.
     */
    template <class... Containers>
    class indexed_zipper_iterator {
            using base_iter = zipper_iterator<Containers...>;
        public:
            using value_type = std::tuple<size_t, typename std::remove_reference_t<decltype((*std::declval<decltype(std::declval<Containers &>().begin())&>()))>...>; ///< Tuple of (index, component values).
            using reference = value_type;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            /**
             * @brief Constructs an indexed zipper iterator.
             *
             * @param it Starting position of the underlying zipper iterator.
             * @param end End position of the underlying zipper iterator.
             */
            indexed_zipper_iterator(base_iter it, base_iter end)
                : _it(it), _end(end), _index(0) {}

            /**
             * @brief Prefix increment operator.
             *
             * @return Reference to this iterator after incrementing.
             */
            indexed_zipper_iterator &operator++() { ++_it; ++_index; return *this; }

            /**
             * @brief Postfix increment operator.
             *
             * @return Reference to this iterator after incrementing.
             */
            indexed_zipper_iterator &operator++(int) { ++_it; ++_index; return *this; }

            /**
             * @brief Dereference operator.
             *
             * @return Tuple containing the entity index followed by component values.
             */
            value_type operator*() { return std::tuple_cat(std::make_tuple(_index), *_it); }

            friend bool operator==(indexed_zipper_iterator const &l, indexed_zipper_iterator const &r) { return l._it == r._it; }
            friend bool operator!=(indexed_zipper_iterator const &l, indexed_zipper_iterator const &r) { return !(l == r); }

        private:
            base_iter _it;   ///< Underlying zipper iterator.
            base_iter _end;  ///< End position marker.
            size_t _index;   ///< Current entity index.
    };



    /**
     * @class indexed_zipper
     * @brief Container adapter that provides indexed parallel iteration.
     *
     * Combines the functionality of zipper with entity index tracking,
     * useful for systems that need both component data and entity IDs.
     *
     * @tparam Containers Container types to zip together.
     */
    template <class... Containers>
    class indexed_zipper {
        public:
            using iterator = indexed_zipper_iterator<Containers...>; ///< Indexed iterator type.
            using base = zipper<Containers...>; ///< Underlying zipper type.

            /**
             * @brief Constructs an indexed zipper from multiple containers.
             *
             * @param cs References to the containers to iterate over.
             */
            indexed_zipper(Containers &...cs) : _base(cs...) {}

            /**
             * @brief Returns an iterator to the beginning.
             *
             * @return Indexed iterator pointing to the first valid entry.
             */
            iterator begin() { return iterator(_base.begin(), _base.end()); }

            /**
             * @brief Returns an iterator to the end.
             *
             * @return Indexed iterator representing the end of iteration.
             */
            iterator end() { return iterator(_base.end(), _base.end()); }

        private:
            base _base; ///< Underlying zipper instance.
    };

} // namespace ecs::containers