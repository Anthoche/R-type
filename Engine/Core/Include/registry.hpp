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

// registry: coeur de l'ECS. Stocke les tableaux de composants par type,
// gère le cycle de vie des entités, et orchestre l'exécution des systèmes.
class registry {
public:
	registry() = default;
	~registry() = default;
	registry(registry const &) = delete;
	registry &operator=(registry const &) = delete;
	registry(registry &&) noexcept = default;
	registry &operator=(registry &&) noexcept = default;

	// Enregistre un type de composant et renvoie son sparse_array associé.
	// Enregistre aussi une fonction d'effacement pour kill_entity.
	template <class Component>
	sparse_array<Component> &register_component() {
		const std::type_index key = std::type_index(typeid(Component));
		auto it = _components_arrays.find(key);
		if (it == _components_arrays.end()) {
			it = _components_arrays.emplace(key, std::any{sparse_array<Component>{}}).first;
			// Effacement du composant Component lors d'un kill_entity
			_erasers.emplace_back([](registry &r, entity_t const &e) {
				std::size_t idx = static_cast<std::size_t>(e);
				r.get_components<Component>().erase(idx);
			});
		}
		return std::any_cast<sparse_array<Component> &>(it->second);
	}

	// Accès au sparse_array d'un type (mutable ou const).
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

	// Systèmes: enregistre une fonction/lambda/foncteur à appeler à chaque frame.
	// Les composants requis sont injectés automatiquement (par type template).
	template <class... Components, typename Function>
	void add_system(Function &&f) {
		using Fn = std::decay_t<Function>;
		_systems.emplace_back([this, func = Fn(std::forward<Function>(f))](registry &r) {
			func(r, static_cast<sparse_array<Components> &>(r.get_components<Components>())...);
		});
	}

	// Définie dans registry.cpp
	void run_systems();

	// Définies dans registry.cpp
	entity_t spawn_entity();
	entity_t entity_from_index(std::size_t idx);
	void kill_entity(entity_t const &e);

	// Gestion des composants pour une entité donnée
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

private:
	// Stockage interne: map type -> any(sparse_array<T>)
	std::unordered_map<std::type_index, std::any> _components_arrays{};
	// Systèmes enregistrés
	std::vector<std::function<void(registry &)>> _systems{};
	// Fonctions d'effacement par type pour kill_entity
	std::vector<std::function<void(registry &, entity_t const &)>> _erasers{};
	// IDs libres et prochain ID
	std::queue<std::size_t> _free_ids{};
	std::size_t _next_entity_id{0};
};

} // namespace ecs
