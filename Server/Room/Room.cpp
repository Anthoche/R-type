/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Room
*/

#include "Room.hpp"

#include "Rtype_game.hpp"

Room::Room(Connexion &connexion, int maxPlayers, int minPlayers, std::string const &game, int roomHost) : _connexion(connexion) {
	_game = std::make_shared<ServerGame>(_connexion);
	_gameStatus = GameStatus::WAITING_PLAYERS;
	_maxPlayers = maxPlayers;
	_minPlayers = minPlayers;
	_gameName = game;
	_roomHost = roomHost;
}

int Room::getMaxPlayers() const {
	return _maxPlayers;
}

int Room::getMinPlayers() const {
	return _minPlayers;
}

GameStatus Room::getGameStatus() const {
	return _gameStatus;
}

std::shared_ptr<IServerGame> Room::getGame() {
	return _game;
}

std::string Room::getGameName() const {
	return _gameName;
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

bool Room::isReady() const {
	return _clients.size() >= _minPlayers;
}

bool Room::isClientInRoom(uint32_t clientId) const {
	for (auto const &client: _clients) {
		if (client == clientId)
			return true;
	}
	return false;
}

int Room::getRoomHost() const {
	return _roomHost;
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

void Room::setGameStatus(GameStatus status) {
	_gameStatus = status;
}

void Room::startGame(int roomId) {
	_game->run(roomId);
}
