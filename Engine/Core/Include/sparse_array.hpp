/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** Sparse array container
*/

#pragma once

#include <vector>
#include <optional>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace ecs {

/**
 * @class sparse_array
 * @brief Sparse container for storing components by entity index.
 *
 * Each index corresponds to an entity identifier.
 * If the entity has no component of the given type, the value at that index is std::nullopt.
 *
 * This allows direct O(1) access to components while keeping memory proportional
 * to the number of active entities.
 *
 * @tparam Component Type of the component stored in the array.
 */
template <typename Component>
class sparse_array {
public:
    using value_type = std::optional<Component>;              ///< Optional component per entity.
    using reference_type = value_type &;                      ///< Mutable reference to a component slot.
    using const_reference_type = value_type const &;          ///< Const reference to a component slot.
    using container_t = std::vector<value_type>;              ///< Underlying storage type.
    using size_type = typename container_t::size_type;        ///< Size/index type.

    using iterator = typename container_t::iterator;          ///< Mutable iterator.
    using const_iterator = typename container_t::const_iterator; ///< Const iterator.

public:
    // === Construction / assignment ===

    sparse_array() = default;
    sparse_array(sparse_array const &) = default;
    sparse_array(sparse_array &&) noexcept = default;
    ~sparse_array() = default;

    sparse_array &operator=(sparse_array const &) = default;
    sparse_array &operator=(sparse_array &&) noexcept = default;

    // === Access ===

    /**
     * @brief Access component slot at given index (auto-resizes if necessary).
     * @param idx Entity index.
     * @return Mutable reference to the optional component.
     */
    reference_type operator[](size_t idx) { ensure_size(idx + 1); return _data[idx]; }

    /**
     * @brief Access component slot at given index (const).
     * @param idx Entity index.
     * @return Const reference to the optional component.
     *         If out of bounds, returns a static empty optional.
     */
    const_reference_type operator[](size_t idx) const { return idx < _data.size() ? _data[idx] : _nullopt; }

    // === Iteration ===

    iterator begin() { return _data.begin(); }
    const_iterator begin() const { return _data.begin(); }
    const_iterator cbegin() const { return _data.cbegin(); }

    iterator end() { return _data.end(); }
    const_iterator end() const { return _data.end(); }
    const_iterator cend() const { return _data.cend(); }

    // === Capacity ===

    /**
     * @brief Get the number of slots allocated (may exceed number of active components).
     * @return Current allocated size.
     */
    size_type size() const { return _data.size(); }

    // === Modifiers ===

    /**
     * @brief Insert or replace a component at the given index.
     * @param pos Entity index.
     * @param c Component instance to insert.
     * @return Reference to the inserted component slot.
     */
    reference_type insert_at(size_type pos, Component const &c) {
        ensure_size(pos + 1);
        _data[pos] = c;
        return _data[pos];
    }

    /**
     * @brief Insert or replace a component at the given index (move version).
     * @param pos Entity index.
     * @param c Component instance to insert (moved).
     * @return Reference to the inserted component slot.
     */
    reference_type insert_at(size_type pos, Component &&c) {
        ensure_size(pos + 1);
        _data[pos] = std::move(c);
        return _data[pos];
    }

    /**
     * @brief Construct a component in place at the given index.
     * @tparam Params Constructor parameter types.
     * @param pos Entity index.
     * @param params Parameters to forward to the Component constructor.
     * @return Reference to the emplaced component slot.
     */
    template <class... Params>
    reference_type emplace_at(size_type pos, Params &&...params) {
        ensure_size(pos + 1);
        _data[pos].reset();
        _data[pos].emplace(Component{std::forward<Params>(params)...});
        return _data[pos];
    }

    /**
     * @brief Remove a component from the given index.
     * @param pos Entity index.
     *
     * The slot remains allocated but becomes empty (std::nullopt).
     */
    void erase(size_type pos) {
        if (pos < _data.size()) {
            _data[pos].reset();
        }
    }

    // === Utilities ===

    /**
     * @brief Get the index of a given reference belonging to this container.
     * @param ref Reference to an optional component in this container.
     * @return The index of the referenced slot, or -1 if not found.
     */
    size_type get_index(value_type const &ref) const {
        auto ptr = std::addressof(ref);
        auto base = _data.data();
        if (ptr >= base && ptr < base + _data.size()) {
            return static_cast<size_type>(ptr - base);
        }
        return static_cast<size_type>(-1);
    }

private:
    /**
     * @brief Ensure the underlying storage has at least `n` slots.
     * @param n Minimum required size.
     */
    void ensure_size(size_type n) {
        if (_data.size() < n) _data.resize(n);
    }

private:
    container_t _data{};                ///< Underlying storage of optional components.
    static inline const value_type _nullopt{}; ///< Static empty optional for out-of-bounds access.
};

} // namespace ecs
