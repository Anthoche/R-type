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

std::shared_ptr<Room> RoomManager::getRoom(int id) {
	return _rooms.at(id);
}

std::map<int, std::shared_ptr<Room>> &RoomManager::getRooms() {
	return _rooms;
}

void RoomManager::addRoom(Room const &room) {
	if (_rooms.size() >= _maxRooms)
		return;
	_rooms.insert_or_assign(_currentId, std::make_shared<Room>(room));
	_currentId++;
}

void RoomManager::addClientInRoom(uint32_t clientId, int roomId) {
	if (!roomExists(roomId))
		return;

	auto room = getRoom(roomId);

	for (auto &r: _rooms) {
		for (auto &p: r.second->getClients()) {
			if (p == clientId)
				r.second->removeClient(clientId);
		}
	}
	room->addClient(clientId);
}

void RoomManager::removeRoom(int id) {
	if (!roomExists(id))
		return;
	_rooms.erase(id);
}

void RoomManager::clearRooms() {
	_rooms.clear();
}
