/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Room
*/

#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../Include/IServerGame.hpp"
#include "GameStatus.hpp"
#include "../Include/connexion.hpp"

class IServerGame;

class Room {
	private:
		Connexion &_connexion;
		int _maxPlayers;
		int _minPlayers;
		std::string _gameName;
		GameStatus _gameStatus;
		std::shared_ptr<IServerGame> _game;
		std::vector<uint32_t> _clients;
		int _roomHost;

	public:
		Room(Connexion &connexion, int maxPlayers, int minPlayers, std::string const &gameName, int _roomHost = -1);
		~Room() = default;

		int getMaxPlayers() const;
		int getMinPlayers() const;
		GameStatus getGameStatus() const;
		std::shared_ptr<IServerGame> getGame();
		std::string getGameName() const;
		std::vector<uint32_t> getClients() const;
		bool isFull() const;
		bool isEmpty() const;
		bool isReady() const;
		bool isClientInRoom(uint32_t clientId) const;
		int getRoomHost() const;

		void addClient(uint32_t clientId);
		void removeClient(uint32_t clientId);
		void setMaxPlayers(int value);
		void setMinPlayers(int value);
		void setGameStatus(GameStatus status);
		void startGame(int roomId);

		//TODO: Run one instance of a game for each room in a separate thread
};
