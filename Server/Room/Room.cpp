/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Room
*/

#include "Room.hpp"

Room::Room(int maxPlayers, int minPlayers, std::string const &game) {
	_maxPlayers = maxPlayers;
	_minPlayers = minPlayers;
	_game = game;
}

int Room::getMaxPlayers() const {
	return _maxPlayers;
}

int Room::getMinPlayers() const {
	return _minPlayers;
}

std::string Room::getGame() const {
	return _game;
}

std::vector<uint32_t> Room::getClients() const {
	return _clients;
}

bool Room::isFull() const {
	return _maxPlayers == _clients.size();
}

bool Room::isEmpty() const {
	return _clients.empty();
}

bool Room::isClientInRoom(uint32_t clientId) const {
	for (auto const &client: _clients) {
		if (client == clientId)
			return true;
	}
	return false;
}

void Room::addClient(u_int32_t clientId) {
	if (isClientInRoom(clientId) || isFull())
		return;
	_clients.push_back(clientId);
}

void Room::removeClient(u_int32_t clientId) {
	for (auto it = _clients.begin(); it != _clients.end(); ++it) {
		if (*it == clientId) {
			_clients.erase(it);
			return;
		}
	}
}

void Room::setMaxPlayers(int value) {
	_maxPlayers = value;
}

void Room::setMinPlayers(int value) {
	_minPlayers = value;
}
