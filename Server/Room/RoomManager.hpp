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

class RoomManager {
	private:
		std::map<int, std::shared_ptr<Room>> _rooms;
		int _currentId;
		int _maxRooms;

	public:
		RoomManager(int maxRooms = 5);
		~RoomManager() = default;

		int getMaxRooms() const;
		std::shared_ptr<Room> getRoom(int id);
		std::map<int, std::shared_ptr<Room>> &getRooms();
		bool roomExists(int id) const;

		void addRoom(Room const &room);
		void addClientInRoom(uint32_t clientId, int roomId);
		void removeRoom(int id);
		void clearRooms();
		void startRoom(int id);
};
