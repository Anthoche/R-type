/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** entity
*/

#include "Include/entity.hpp"

namespace ecs {

entity_t::entity_t(std::size_t id) noexcept : _id(id) {}

entity_t::operator std::size_t() const noexcept { return _id; }

std::size_t entity_t::value() const noexcept { return _id; }

bool operator==(const entity_t &lhs, const entity_t &rhs) noexcept { return lhs._id == rhs._id; }

bool operator!=(const entity_t &lhs, const entity_t &rhs) noexcept { return !(lhs == rhs); }

}
