/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** serializer
*/

#pragma once

#include <nlohmann/json.hpp>
#include "../../Core/Include/registry.hpp"
#include "../../Core/Entities/Include/components.hpp"
#include "entity_parser.hpp"
#include "../../Server/Room/Room.hpp"

namespace game::serializer {
	nlohmann::json serialize_entity(ecs::registry &reg, ecs::entity_t entity);

	void deserialize_entities(ecs::registry &reg, const nlohmann::json &j);

	struct RoomData {
		std::string gameName;
		GameStatus status;
		int minPlayers;
		int maxPlayers;
		int currentPlayers;
	};

	nlohmann::json serialize_rooms(std::map<int, RoomData> const &rooms);

	std::map<int, RoomData> deserialize_rooms(nlohmann::json const &json);
} // namespace game::net::serializer
