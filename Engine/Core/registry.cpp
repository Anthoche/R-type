/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** registry
*/

#include "Include/registry.hpp"
#include <unordered_set>

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

entity_t registry::entity_from_index(std::size_t idx) const{
    return entity_t{idx};
}

void registry::kill_entity(entity_t const &e) {
    for (auto &erase_fn : _erasers) {
        erase_fn(*this, e);
    }
    _free_ids.push(static_cast<std::size_t>(e));
}

std::vector<entity_t> registry::alive_entities() const {
    std::vector<entity_t> result;
    result.reserve(_next_entity_id - _free_ids.size());
    std::unordered_set<std::size_t> dead;
    std::queue<std::size_t> copy = _free_ids;

    while (!copy.empty()) {
        dead.insert(copy.front());
        copy.pop();
    }
    for (std::size_t i = 0; i < _next_entity_id; ++i) {
        if (dead.find(i) == dead.end()) {
            result.push_back(entity_from_index(i));
        }
    }
    return result;
}

} // namespace ecs
