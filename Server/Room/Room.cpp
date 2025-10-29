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
	_isGameServerStarted = false;
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

std::map<uint32_t, bool> Room::getClients() const {
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
		if (client.first == clientId)
			return true;
	}
	return false;
}

int Room::getRoomHost() const {
	return _roomHost;
}

bool Room::isGameServerStarted() const {
	return _isGameServerStarted;
}

void Room::setClientConfirmed(uint32_t clientId) {
	if (!isClientInRoom(clientId)) return;
	_clients[clientId] = true;
}

void Room::addClient(u_int32_t clientId) {
	if (isClientInRoom(clientId) || isFull())
		return;
	_clients.insert_or_assign(clientId, false);
}

void Room::removeClient(u_int32_t clientId) {
	for (auto it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->first == clientId) {
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
	_isGameServerStarted = true;
	_game->setInitialClients(_clients);
	_game->run(roomId);
}
