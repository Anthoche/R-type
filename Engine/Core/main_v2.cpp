/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** main
*/

#include <iostream>
#include <algorithm>
#include "Include/registry.hpp"
#include "Include/zipper.hpp"
#include "../../Game/Entities/Include/components.hpp"

int main_v2() {
	ecs::registry r;

	// Enregistre les types de composants dans le registry (crée leurs sparse_arrays)
	auto &positions = r.register_component<component::position>();
	auto &velocities = r.register_component<component::velocity>();

	// Système 1: parcours naïf et log des entités ayant position+velocity
	r.add_system<component::position, component::velocity>(
		[](ecs::registry &reg,
		   ecs::sparse_array<component::position> const &pos,
		   ecs::sparse_array<component::velocity> const &vel) {
			const std::size_t n = std::max(pos.size(), vel.size());
			for (std::size_t i = 0; i < n; ++i) {
				if (i < pos.size() && i < vel.size() && pos[i] && vel[i]) {
					std::cout << i << ": Position={" << pos[i]->x << "," << pos[i]->y
					          << "} Velocity={" << vel[i]->vx << "," << vel[i]->vy << "}\n";
				}
			}
		});

	// Système 2: version avec indexed_zipper (itération parallèle + index)
	r.add_system<component::position, component::velocity>(
		[](ecs::registry &reg,
		   ecs::sparse_array<component::position> const &pos,
		   ecs::sparse_array<component::velocity> const &vel) {
			using ecs::containers::indexed_zipper;
			for (auto tpl : indexed_zipper(pos, vel)) {
				auto [i, p, v] = tpl;
				if (p && v) {
					std::cout << "idx=" << i << " -> Pos{" << p->x << "," << p->y
					          << "} Vel{" << v->vx << "," << v->vy << "}\n";
				}
			}
		});

	// ---- Test intégration Step 2.3 ----
	// Création de 3 entités
	ecs::entity_t e0 = r.spawn_entity();
	ecs::entity_t e1 = r.spawn_entity();
	ecs::entity_t e2 = r.spawn_entity();

	// Ajout de composants (insert)
	r.add_component<component::position>(e0, component::position{10.0f, 20.0f});
	r.add_component<component::velocity>(e0, component::velocity{1.0f, 2.0f});
	r.add_component<component::position>(e1, component::position{30.0f, 40.0f}); // pas de velocity
	// Construction en place (emplace)
	r.emplace_component<component::position>(e2, 50.0f, 60.0f);
	r.emplace_component<component::velocity>(e2, -3.0f, 4.0f);

	std::cout << "--- Run #1 ---\n";
	r.run_systems();

	// Suppression d'un composant et re-run
	r.remove_component<component::velocity>(e2);
	std::cout << "--- Run #2 (after remove velocity of e2) ---\n";
	r.run_systems();

	// Destruction d'une entité et re-run
	r.kill_entity(e0);
	std::cout << "--- Run #3 (after kill e0) ---\n";
	r.run_systems();

	(void)positions; (void)velocities; // évite un-used si optimisation
	return 0;
}
