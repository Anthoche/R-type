/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomManager
*/

#pragma once

#include <cstdint>
#include "Room.hpp"
#include <map>
#include <optional>

/**
 * @class RoomManager
 * @brief Manages all game rooms on the server.
 *
 * The RoomManager is responsible for creating, tracking, and destroying game rooms.
 * It maintains a registry of active rooms, handles room capacity limits, and provides
 * utilities for finding rooms by ID or by client membership. The manager ensures
 * proper resource allocation and prevents exceeding the maximum number of concurrent rooms.
 */
class RoomManager {
	private:
		std::map<int, std::shared_ptr<Room>> _rooms; ///< Maps room IDs to room instances.
		int _currentId; ///< Counter for generating unique room IDs.
		int _maxRooms; ///< Maximum number of concurrent rooms allowed.

	public:
		/**
		 * @brief Constructs a RoomManager with a specified maximum room capacity.
		 *
		 * @param maxRooms Maximum number of rooms that can exist simultaneously (default: 5).
		 */
		RoomManager(int maxRooms = 5);

		/**
		 * @brief Destructor that cleans up all rooms.
		 */
		~RoomManager() = default;

		/**
		 * @brief Gets the maximum number of rooms allowed.
		 *
		 * @return Maximum room capacity.
		 */
		int getMaxRooms() const;

		/**
		 * @brief Retrieves a room by its unique ID.
		 *
		 * @param id The room ID to look up.
		 * @return Shared pointer to the Room, or nullptr if not found.
		 */
		std::shared_ptr<Room> getRoom(int id);

		/**
		 * @brief Gets the map of all active rooms.
		 *
		 * @return Reference to the map of room IDs to room instances.
		 */
		std::map<int, std::shared_ptr<Room>> &getRooms();

		/**
		 * @brief Checks if a room with the given ID exists.
		 *
		 * @param id The room ID to check.
		 * @return true if the room exists, false otherwise.
		 */
		bool roomExists(int id) const;

		/**
		 * @brief Finds the room ID that contains a specific client.
		 *
		 * Searches through all rooms to find which one the client is currently in.
		 *
		 * @param clientId The client ID to search for.
		 * @return Optional containing the room ID if found, or empty if the client is not in any room.
		 */
		std::optional<int> findRoomIdByClient(uint32_t clientId) const;

		/**
		 * @brief Creates and registers a new room.
		 *
		 * Adds a new room to the manager with an automatically generated unique ID.
		 * Fails if the maximum number of rooms has been reached.
		 *
		 * @param room The room configuration to add.
		 */
		void addRoom(Room const &room);

		/**
		 * @brief Adds a client to a specific room.
		 *
		 * Assigns a player to the specified room if it exists and has available space.
		 *
		 * @param clientId The client ID to add to the room.
		 * @param roomId The target room ID.
		 */
		void addClientInRoom(uint32_t clientId, int roomId);

		/**
		 * @brief Removes a room from the manager.
		 *
		 * Deletes the specified room and frees its resources.
		 *
		 * @param id The room ID to remove.
		 */
		void removeRoom(int id);

		/**
		 * @brief Removes all rooms from the manager.
		 *
		 * Clears the entire room registry, typically used during server shutdown.
		 */
		void clearRooms();

		/**
		 * @brief Starts the game for a specific room.
		 *
		 * Initiates gameplay for the specified room if it meets the ready conditions.
		 *
		 * @param id The room ID to start.
		 */
		void startRoom(int id);
};