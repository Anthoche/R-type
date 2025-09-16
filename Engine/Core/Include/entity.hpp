#pragma once

#include <cstddef>
#include <utility>

namespace ecs {

// Représente une entité via un identifiant numérique.
// Construction explicite, conversion implicite vers size_t permise.
class entity_t {
public:
	// Construit une entité à partir d'un identifiant.
	explicit entity_t(std::size_t id) noexcept;

	// Conversion implicite pour récupérer l'identifiant.
	operator std::size_t() const noexcept;

	// Accès explicite à l'identifiant.
	std::size_t value() const noexcept;

	// Comparaisons par identifiant (définies hors classe, amies pour accès privé).
	friend bool operator==(const entity_t &lhs, const entity_t &rhs) noexcept;
	friend bool operator!=(const entity_t &lhs, const entity_t &rhs) noexcept;
private:
	std::size_t _id; // identifiant interne
};

} // namespace ecs