#pragma once

#include <tuple>
#include <optional>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace ecs::containers {

// Déclaration avancée: conteneur adaptateur pour itérer en parallèle
// sur plusieurs sparse_arrays en ne gardant que les indices valides.
template <class... Containers>
class zipper;

// Iterateur qui avance simultanément sur N conteneurs et ne renvoie
// que les tuples de valeurs lorsque toutes les optionals sont présentes.
template <class... Containers>
class zipper_iterator {
	template <class Container>
	using iterator_t = decltype(std::declval<Container &>().begin());

	template <class Container>
	using it_reference_t = typename std::iterator_traits<iterator_t<Container>>::reference;

public:
	using value_type = std::tuple<typename std::remove_reference_t<decltype((*std::declval<iterator_t<Containers>&>()))>...>;
	using reference = value_type;
	using pointer = void;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::input_iterator_tag;
	using iterator_tuple = std::tuple<iterator_t<Containers>...>;

	template <class... Cs>
	friend class zipper;

	// Construit l'itérateur à partir d'un tuple d'itérateurs et d'une borne max.
	zipper_iterator(iterator_tuple const &it_tuple, size_t max)
		: _current(it_tuple), _max(max), _idx(0) {
		// Avancer au premier ensemble valide
		if (_max > 0) {
			while (_idx < _max && !all_set(std::index_sequence_for<Containers...>{})) {
				incr_all(std::index_sequence_for<Containers...>{});
			}
		}
	}

	zipper_iterator(zipper_iterator const &z) = default;

	// Incrémente en sautant les cases manquantes
	zipper_iterator operator++() { incr_all(std::index_sequence_for<Containers...>{}); return *this; }
	zipper_iterator &operator++(int) { incr_all(std::index_sequence_for<Containers...>{}); return *this; }

	// Déréférencement -> tuple de références optional<...>
	value_type operator*() { return to_value(std::index_sequence_for<Containers...>{}); }
	value_type operator->() { return to_value(std::index_sequence_for<Containers...>{}); }

	friend bool operator==(zipper_iterator const &lhs, zipper_iterator const &rhs) {
		return lhs._current == rhs._current;
	}
	friend bool operator!=(zipper_iterator const &lhs, zipper_iterator const &rhs) { return !(lhs == rhs); }

private:
	// Avance tous les itérateurs en parallèle, saute jusqu'à trouver tous_set
	template <size_t... Is>
	void incr_all(std::index_sequence<Is...>) {
		(++std::get<Is>(_current), ...);
		++_idx;
		while (_idx < _max && !all_set(std::index_sequence_for<Containers...>{})) {
			(++std::get<Is>(_current), ...);
			++_idx;
		}
	}

	// Vérifie que toutes les optionals pointées contiennent une valeur
	template <size_t... Is>
	bool all_set(std::index_sequence<Is...>) {
		bool ok = true;
		(void)std::initializer_list<int>{(ok = ok && static_cast<bool>(*std::get<Is>(_current)), 0)...};
		return ok;
	}

	// Construit le tuple de valeurs déréférencées
	template <size_t... Is>
	value_type to_value(std::index_sequence<Is...>) {
		return value_type{(*std::get<Is>(_current))...};
	}

private:
	iterator_tuple _current;
	size_t _max;
	size_t _idx;
};

// Conteneur adaptateur produisant zipper_iterator
template <class... Containers>
class zipper {
public:
	using iterator = zipper_iterator<Containers...>;
	using iterator_tuple = typename iterator::iterator_tuple;

	zipper(Containers &...cs)
		: _begin(std::make_tuple(cs.begin()...)),
		  _end(_compute_end(cs...)),
		  _size(_compute_size(cs...)) {
	}

	iterator begin() { iterator it(_begin, _size); return it; }
	iterator end() { return iterator(_end, _size); }

private:
	static size_t _compute_size(Containers &...containers) { return std::min({static_cast<size_t>(containers.size())...}); }
	static iterator_tuple _compute_end(Containers &...containers) { return std::make_tuple(containers.end()...); }

private:
	iterator_tuple _begin;
	iterator_tuple _end;
	size_t _size;
};

// Variante indexée: renvoie (index, ...tuple de valeurs...)
template <class... Containers>
class indexed_zipper_iterator {
	using base_iter = zipper_iterator<Containers...>;
public:
	using value_type = std::tuple<size_t, typename std::remove_reference_t<decltype((*std::declval<decltype(std::declval<Containers &>().begin())&>()))>...>;
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
	base_iter _it;
	base_iter _end;
	size_t _index;
};

template <class... Containers>
class indexed_zipper {
public:
	using iterator = indexed_zipper_iterator<Containers...>;
	using base = zipper<Containers...>;

	indexed_zipper(Containers &...cs) : _base(cs...) {}
	iterator begin() { return iterator(_base.begin(), _base.end()); }
	iterator end() { return iterator(_base.end(), _base.end()); }

private:
	base _base;
};

}
