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
#include "GameStatus.hpp"

/**
 * @namespace game::serializer
 * @brief Contains functions for serializing and deserializing game data.
 *
 * This namespace provides utilities to convert ECS entities and game state
 * to and from JSON format for network transmission and storage.
 */
namespace game::serializer {
	/**
	 * @brief Serializes a single entity and its components to JSON format.
	 *
	 * Converts an entity's components into a JSON object that can be transmitted
	 * over the network or saved to disk.
	 *
	 * @param reg Reference to the ECS registry containing the entity.
	 * @param entity The entity identifier to serialize.
	 * @return JSON object representing the entity and its components.
	 */
	nlohmann::json serialize_entity(ecs::registry &reg, ecs::entity_t entity);

	/**
	 * @brief Deserializes entities from JSON and creates them in the registry.
	 *
	 * Parses a JSON object containing entity data and recreates the entities
	 * with their components in the provided ECS registry.
	 *
	 * @param reg Reference to the ECS registry where entities will be created.
	 * @param j JSON object containing serialized entity data.
	 */
	void deserialize_entities(ecs::registry &reg, const nlohmann::json &j);

	/**
	 * @struct RoomData
	 * @brief Contains information about a multiplayer game room.
	 */
	struct RoomData {
		std::string gameName;    ///< Name of the game/room.
		GameStatus status;       ///< Current status of the room (waiting, playing, etc.).
		int minPlayers;          ///< Minimum number of players required to start.
		int maxPlayers;          ///< Maximum number of players allowed.
		int currentPlayers;      ///< Current number of players in the room.
	};

	/**
	 * @brief Serializes a collection of game rooms to JSON format.
	 *
	 * Converts a map of room data to JSON for transmission to clients
	 * requesting the list of available game rooms.
	 *
	 * @param rooms Map of room IDs to their corresponding room data.
	 * @return JSON object containing serialized room information.
	 */
	nlohmann::json serialize_rooms(std::map<int, RoomData> const &rooms);

	/**
	 * @brief Deserializes game rooms from JSON format.
	 *
	 * Parses JSON data containing room information and reconstructs
	 * a map of available game rooms.
	 *
	 * @param json JSON object containing serialized room data.
	 * @return Map of room IDs to their corresponding room data.
	 */
	std::map<int, RoomData> deserialize_rooms(nlohmann::json const &json);
} // namespace game::serializer