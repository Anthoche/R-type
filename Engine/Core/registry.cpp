#include "Include/registry.hpp"

namespace ecs {

void registry::run_systems() {
	for (auto &sys : _systems) {
		sys(*this);
	}
}

entity_t registry::spawn_entity() {
	std::size_t id;
	if (!_free_ids.empty()) {
		id = _free_ids.front();
		_free_ids.pop();
	} else {
		id = _next_entity_id++;
	}
	return entity_t{id};
}

entity_t registry::entity_from_index(std::size_t idx) { return entity_t{idx}; }

void registry::kill_entity(entity_t const &e) {
	for (auto &erase_fn : _erasers) {
		erase_fn(*this, e);
	}
	_free_ids.push(static_cast<std::size_t>(e));
}

} // namespace ecs
