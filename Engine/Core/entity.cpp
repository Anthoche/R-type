#include "Include/entity.hpp"

namespace ecs {

entity_t::entity_t(std::size_t id) noexcept : _id(id) {}

entity_t::operator std::size_t() const noexcept { return _id; }

std::size_t entity_t::value() const noexcept { return _id; }

bool operator==(const entity_t &lhs, const entity_t &rhs) noexcept { return lhs._id == rhs._id; }

bool operator!=(const entity_t &lhs, const entity_t &rhs) noexcept { return !(lhs == rhs); }

} // namespace ecs
