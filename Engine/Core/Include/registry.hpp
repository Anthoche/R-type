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

        /**
         * @brief Registers a new component type in the registry.
         *
         * Also registers a component eraser used when killing an entity.
         *
         * @tparam Component The type of the component.
         * @return Reference to the sparse_array of that component type.
         */
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

        /**
         * @brief Access the sparse array for a component type (mutable).
         * @tparam Component Component type.
         * @return Reference to the sparse array.
         * @throw std::runtime_error if the component type was not registered.
         */
        template <class Component>
        sparse_array<Component> &get_components() {
            const std::type_index key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end()) {
                throw std::runtime_error("Component type not registered");
            }
            return std::any_cast<sparse_array<Component> &>(it->second);
        }

        /**
         * @brief Access the sparse array for a component type (const).
         * @tparam Component Component type.
         * @return Const reference to the sparse array.
         * @throw std::runtime_error if the component type was not registered.
         */
        template <class Component>
        sparse_array<Component> const &get_components() const {
            const std::type_index key = std::type_index(typeid(Component));
            auto it = _components_arrays.find(key);
            if (it == _components_arrays.end()) {
                throw std::runtime_error("Component type not registered");
            }
            return std::any_cast<sparse_array<Component> const &>(it->second);
        }

        /**
         * @brief Adds a system to the registry.
         *
         * The system function/lambda will be called every frame and is automatically injected with
         * sparse arrays of the specified component types.
         *
         * @tparam Components Component types required by the system.
         * @tparam Function Function/lambda type.
         * @param f The system function.
         */
        template <class... Components, typename Function>
        void add_system(Function &&f) {
            using Fn = std::decay_t<Function>;
            _systems.emplace_back([this, func = Fn(std::forward<Function>(f))](registry &r) {
                func(r, static_cast<sparse_array<Components> &>(r.get_components<Components>())...);
            });
        }

        /** @brief Runs all registered systems. Defined in registry.cpp */
        void run_systems();

        /** @brief Creates a new entity and returns it. Defined in registry.cpp */
        entity_t spawn_entity();

        /** @brief Retrieves an entity from its index. Defined in registry.cpp */
        entity_t entity_from_index(std::size_t idx);

        /** @brief Kills an entity, removing all its components. Defined in registry.cpp */
        void kill_entity(entity_t const &e);

        /**
         * @brief Adds a component to an entity.
         * @tparam Component Component type.
         * @param to Entity to add the component to.
         * @param c Component instance.
         * @return Reference to the stored component.
         */
        template <typename Component>
        typename sparse_array<Component>::reference_type add_component(entity_t const &to, Component &&c) {
            auto &arr = get_components<Component>();
            return arr.insert_at(static_cast<std::size_t>(to), std::forward<Component>(c));
        }

        /**
         * @brief Emplaces a component in-place for an entity.
         * @tparam Component Component type.
         * @tparam Params Constructor parameters for the component.
         * @param to Entity to add the component to.
         * @param p Parameters to construct the component.
         * @return Reference to the stored component.
         */
        template <typename Component, typename... Params>
        typename sparse_array<Component>::reference_type emplace_component(entity_t const &to, Params &&...p) {
            auto &arr = get_components<Component>();
            return arr.emplace_at(static_cast<std::size_t>(to), std::forward<Params>(p)...);
        }

        /**
         * @brief Removes a component from an entity.
         * @tparam Component Component type.
         * @param from Entity to remove the component from.
         */
        template <typename Component>
        void remove_component(entity_t const &from) {
            auto &arr = get_components<Component>();
            arr.erase(static_cast<std::size_t>(from));
        }

    private:
        std::unordered_map<std::type_index, std::any> _components_arrays; ///< Map from component type to sparse array
        std::vector<std::function<void(registry &)>> _systems; ///< Registered systems
        std::vector<std::function<void(registry &, entity_t const &)>> _erasers; ///< Erasers for kill_entity
        std::queue<std::size_t> _free_ids; ///< Free entity IDs
        std::size_t _next_entity_id{0}; ///< Next available entity ID
    };

} // namespace ecs
