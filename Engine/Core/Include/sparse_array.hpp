#pragma once

#include <vector>
#include <optional>
#include <cstddef>
#include <memory>
#include <type_traits>

namespace ecs {

// Conteneur clairsemé pour stocker des composants de type Component.
// Chaque index correspond à une entité; absence de composant => std::nullopt.
template <typename Component>
class sparse_array {
public:
	using value_type = std::optional<Component>; // valeur optionnelle par entité
	using reference_type = value_type &;
	using const_reference_type = value_type const &;
	using container_t = std::vector<value_type>;
	using size_type = typename container_t::size_type;

	using iterator = typename container_t::iterator;
	using const_iterator = typename container_t::const_iterator;

public:
	// Construction/gestion par défaut.
	sparse_array() = default;
	sparse_array(sparse_array const &) = default;
	sparse_array(sparse_array &&) noexcept = default;
	~sparse_array() = default;

	sparse_array &operator=(sparse_array const &) = default;
	sparse_array &operator=(sparse_array &&) noexcept = default;

	// Accès (agrandit automatiquement si on accède en écriture via operator[] non-const).
	reference_type operator[](size_t idx) { ensure_size(idx + 1); return _data[idx]; }
	const_reference_type operator[](size_t idx) const { return idx < _data.size() ? _data[idx] : _nullopt; }

	// Itération sur le stockage sous-jacent.
	iterator begin() { return _data.begin(); }
	const_iterator begin() const { return _data.begin(); }
	const_iterator cbegin() const { return _data.cbegin(); }

	iterator end() { return _data.end(); }
	const_iterator end() const { return _data.end(); }
	const_iterator cend() const { return _data.cend(); }

	// Nombre de cases allouées (peut être > au nombre de composants présents).
	size_type size() const { return _data.size(); }

	// Insère/remplace un composant à l'index donné (redimensionne si nécessaire).
	reference_type insert_at(size_type pos, Component const &c) {
		ensure_size(pos + 1);
		_data[pos] = c;
		return _data[pos];
	}

	reference_type insert_at(size_type pos, Component &&c) {
		ensure_size(pos + 1);
		_data[pos] = std::move(c);
		return _data[pos];
	}

	// Construit en place un composant (supporte les agrégats via list-initialization).
	template <class... Params>
	reference_type emplace_at(size_type pos, Params &&...params) {
		ensure_size(pos + 1);
		_data[pos].reset();
		_data[pos].emplace(Component{std::forward<Params>(params)...});
		return _data[pos];
	}

	// Supprime le composant à l'index (la case reste allouée mais devient vide).
	void erase(size_type pos) {
		if (pos < _data.size()) {
			_data[pos].reset();
		}
	}

	// Récupère l'index d'une référence optionnelle appartenant à ce conteneur.
	size_type get_index(value_type const &ref) const {
		auto ptr = std::addressof(ref);
		auto base = _data.data();
		if (ptr >= base && ptr < base + _data.size()) {
			return static_cast<size_type>(ptr - base);
		}
		return static_cast<size_type>(-1);
	}

private:
	// Garantit la capacité nécessaire.
	void ensure_size(size_type n) {
		if (_data.size() < n) _data.resize(n);
	}

private:
	container_t _data{}; // stockage des optionals
	static inline const value_type _nullopt{}; // renvoyé pour operator[] const hors borne
};

} // namespace ecs
