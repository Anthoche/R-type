/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomManager
*/

#pragma once

#include "Room.hpp"
#include <map>

class RoomManager {
	private:
		std::map<int, Room> _rooms;
		long _currentId;
		int _maxRooms;

	public:
		RoomManager(int maxRooms = 5);
		~RoomManager() = default;

		int getMaxRooms() const;
		Room& getRoom(int id);
		std::map<int, Room> const &getRooms() const;
		bool roomExists(int id) const;

		void addRoom(Room const &room);
		void removeRoom(int id);
		void clearRooms();
};
