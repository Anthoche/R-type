/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomManager
*/

#include "RoomManager.hpp"

RoomManager::RoomManager(int maxRooms) {
	_maxRooms = maxRooms;
	_currentId = 0;
}

int RoomManager::getMaxRooms() const {
	return _maxRooms;
}

bool RoomManager::roomExists(int id) const {
	for (auto &room: _rooms) {
		if (room.first == id)
			return true;
	}
	return false;
}

Room &RoomManager::getRoom(int id) {
	return _rooms.at(id);
}

std::map<int, Room> const &RoomManager::getRooms() const {
	return _rooms;
}

void RoomManager::addRoom(Room const &room) {
	if (_rooms.size() >= _maxRooms)
		return;
	_rooms.insert_or_assign(_currentId, room);
	_currentId++;
}

void RoomManager::removeRoom(int id) {
	if (!roomExists(id))
		return;
	_rooms.erase(id);
}

void RoomManager::clearRooms() {
	_rooms.clear();
}
