/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** registry
*/

#pragma once

#include <unordered_map>
#include <typeindex>
#include <any>
#include <stdexcept>
#include <functional>
#include <vector>
#include <type_traits>
#include <queue>

#include "sparse_array.hpp"
#include "entity.hpp"

namespace ecs {

    /**
     * @class registry
     * @brief Core ECS registry that manages entities, components, and systems.
     *
     * The registry stores sparse arrays of components per type, handles entity creation and destruction,
     * and runs systems that operate on entities with specific component combinations.
     */
    class registry {
    public:
        registry() = default;
        ~registry() = default;
        registry(registry const &) = delete;
        registry &operator=(registry const &) = delete;
        registry(registry &&) noexcept = default;
        registry &operator=(registry &&) noexcept = default;

        // --- Component registration ---
        template <class Component>
        sparse_array<Component> &register_component() {
            const std::type_index key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end()) {
                it = _components_arrays.emplace(key, std::any{sparse_array<Component>{}}).first;
                // Register eraser for kill_entity
                _erasers.emplace_back([](registry &r, entity_t const &e) {
                    std::size_t idx = static_cast<std::size_t>(e);
                    r.get_components<Component>().erase(idx);
                });
            }
            return std::any_cast<sparse_array<Component> &>(it->second);
        }

        template <class Component>
        sparse_array<Component> &get_components() {
            const std::type_index key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end()) {
                throw std::runtime_error("Component type not registered");
            }
            return std::any_cast<sparse_array<Component> &>(it->second);
        }

        template <class Component>
        sparse_array<Component> const &get_components() const {
            const std::type_index key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end()) {
                throw std::runtime_error("Component type not registered");
            }
            return std::any_cast<sparse_array<Component> const &>(it->second);
        }

        // --- Systems ---
        template <class... Components, typename Function>
        void add_system(Function &&f) {
            using Fn = std::decay_t<Function>;
            _systems.emplace_back([this, func = Fn(std::forward<Function>(f))](registry &r) {
                func(r, static_cast<sparse_array<Components> &>(r.get_components<Components>())...);
            });
        }

        void run_systems();

        // --- Entities ---
        entity_t spawn_entity();
        entity_t entity_from_index(std::size_t idx) const;
        void kill_entity(entity_t const &e);

        template <typename Component>
        typename sparse_array<Component>::reference_type add_component(entity_t const &to, Component &&c) {
            auto &arr = get_components<Component>();
            return arr.insert_at(static_cast<std::size_t>(to), std::forward<Component>(c));
        }

        template <typename Component, typename... Params>
        typename sparse_array<Component>::reference_type emplace_component(entity_t const &to, Params &&...p) {
            auto &arr = get_components<Component>();
            return arr.emplace_at(static_cast<std::size_t>(to), std::forward<Params>(p)...);
        }

        template <typename Component>
        void remove_component(entity_t const &from) {
            auto &arr = get_components<Component>();
            arr.erase(static_cast<std::size_t>(from));
        }

        /**
         * @brief Récupère toutes les entités encore vivantes (non détruites).
         */
        std::vector<entity_t> alive_entities() const;

    private:
        std::unordered_map<std::type_index, std::any> _components_arrays; ///< Map from component type to sparse array
        std::vector<std::function<void(registry &)>> _systems; ///< Registered systems
        std::vector<std::function<void(registry &, entity_t const &)>> _erasers; ///< Erasers for kill_entity
        std::queue<std::size_t> _free_ids; ///< Free entity IDs
        std::size_t _next_entity_id{0}; ///< Next available entity ID
    };

} // namespace ecs
