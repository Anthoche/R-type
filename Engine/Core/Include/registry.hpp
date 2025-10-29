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
     * The registry is the central hub of the Entity Component System architecture.
     * It stores all components in sparse arrays indexed by component type,
     * manages entity lifecycle (creation and destruction), and executes systems
     * that process entities with specific component combinations.
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

            /**
             * @brief Registers a component type and returns its sparse array.
             *
             * If the component type is already registered, returns the existing sparse array.
             * Otherwise, creates a new sparse array for this component type and registers
             * an eraser function for entity cleanup.
             *
             * @tparam Component The component type to register.
             * @return Reference to the sparse array containing all components of this type.
             */
            template <class Component>
            sparse_array<Component> &register_component()
            {
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
             * @brief Retrieves the sparse array for a component type.
             *
             * @tparam Component The component type to retrieve.
             * @return Reference to the sparse array containing all components of this type.
             * @throws std::runtime_error if the component type is not registered.
             */
            template <class Component>
            sparse_array<Component> &get_components()
            {
                const std::type_index key = std::type_index(typeid(Component));
                auto it = _components_arrays.find(key);
                if (it == _components_arrays.end()) {
                    throw std::runtime_error("Component type not registered");
                }
                return std::any_cast<sparse_array<Component> &>(it->second);
            }

            /**
             * @brief Retrieves the sparse array for a component type (const version).
             *
             * @tparam Component The component type to retrieve.
             * @return Const reference to the sparse array containing all components of this type.
             * @throws std::runtime_error if the component type is not registered.
             */
            template <class Component>
            sparse_array<Component> const &get_components() const
            {
                const std::type_index key = std::type_index(typeid(Component));
                auto it = _components_arrays.find(key);
                if (it == _components_arrays.end()) {
                    throw std::runtime_error("Component type not registered");
                }
                return std::any_cast<sparse_array<Component> const &>(it->second);
            }

            // --- Systems ---

            /**
             * @brief Registers a system function that operates on entities with specific components.
             *
             * Systems are functions that process entities containing all specified component types.
             * The system receives references to the sparse arrays of the requested components.
             *
             * @tparam Components Component types the system operates on.
             * @tparam Function The function type (callable).
             * @param f The system function to register.
             */
            template <class... Components, typename Function>
            void add_system(Function &&f)
            {
                using Fn = std::decay_t<Function>;
                _systems.emplace_back([this, func = Fn(std::forward<Function>(f))](registry &r) {
                    func(r, static_cast<sparse_array<Components> &>(r.get_components<Components>())...);
                });
            }

            /**
             * @brief Executes all registered systems in order.
             *
             * Each system processes entities that have the components it requires.
             */
            void run_systems();

            // --- Entities ---

            /**
             * @brief Creates a new entity and returns its unique identifier.
             *
             * @return The unique identifier of the newly created entity.
             */
            entity_t spawn_entity();

            /**
             * @brief Converts an index to an entity identifier.
             *
             * @param idx The index to convert.
             * @return The entity identifier corresponding to the index.
             */
            entity_t entity_from_index(std::size_t idx) const;

            /**
             * @brief Destroys an entity and removes all its components.
             *
             * The entity ID is recycled and can be reused for future entities.
             *
             * @param e The entity to destroy.
             */
            void kill_entity(entity_t const &e);

            /**
             * @brief Adds a component to an entity by moving it.
             *
             * @tparam Component The component type to add.
             * @param to The entity to add the component to.
             * @param c The component instance to move.
             * @return Reference to the added component.
             */
            template <typename Component>
            typename sparse_array<Component>::reference_type add_component(entity_t const &to, Component &&c)
            {
                auto &arr = get_components<Component>();
                return arr.insert_at(static_cast<std::size_t>(to), std::forward<Component>(c));
            }

            /**
             * @brief Constructs a component in-place for an entity.
             *
             * @tparam Component The component type to construct.
             * @tparam Params Constructor parameter types.
             * @param to The entity to add the component to.
             * @param p Constructor parameters.
             * @return Reference to the constructed component.
             */
            template <typename Component, typename... Params>
            typename sparse_array<Component>::reference_type emplace_component(entity_t const &to, Params &&...p)
            {
                auto &arr = get_components<Component>();
                return arr.emplace_at(static_cast<std::size_t>(to), std::forward<Params>(p)...);
            }

            /**
             * @brief Removes a component from an entity.
             *
             * @tparam Component The component type to remove.
             * @param from The entity to remove the component from.
             */
            template <typename Component>
            void remove_component(entity_t const &from)
            {
                auto &arr = get_components<Component>();
                arr.erase(static_cast<std::size_t>(from));
            }

            /**
             * @brief Retrieves all currently alive entities.
             *
             * @return Vector of entity identifiers for all non-destroyed entities.
             */
            std::vector<entity_t> alive_entities() const;

            /**
             * @brief Completely clears the registry and resets all state.
             *
             * Removes all components, entities, systems, and erasers.
             * Resets entity ID generation to start from 0 again.
             * This is typically used when switching between game scenes or completely resetting the game world.
             */
            void clear()
            {
                _components_arrays.clear();
                _systems.clear();
                _erasers.clear();

                _next_entity_id = 0;
                while (!_free_ids.empty())
                    _free_ids.pop();
            }

        private:
            std::unordered_map<std::type_index, std::any> _components_arrays; ///< Maps component types to their sparse arrays.
            std::vector<std::function<void(registry &)>> _systems; ///< Registered system functions.
            std::vector<std::function<void(registry &, entity_t const &)>> _erasers; ///< Component cleanup functions for entity destruction.
            std::queue<std::size_t> _free_ids; ///< Pool of recycled entity IDs available for reuse.
            std::size_t _next_entity_id{0}; ///< Counter for generating new entity IDs.
        };

} // namespace ecs