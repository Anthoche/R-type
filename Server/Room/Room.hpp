/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Room
*/

#pragma once

#include <string>
#include <vector>
#include "../../Engine/GameStatus.hpp"

class Room {
	private:
		int _maxPlayers;
		int _minPlayers;
		std::string _game;
		std::vector<u_int32_t> _clients;

	public:
		Room(int maxPlayers, int minPlayers, std::string const &game);
		~Room() = default;

		int getMaxPlayers() const;
		int getMinPlayers() const;
		std::string getGame() const;
		std::vector<u_int32_t> getClients() const;
		bool isFull() const;
		bool isEmpty() const;
		bool isClientInRoom(u_int32_t clientId) const;

		void addClient(u_int32_t clientId);
		void removeClient(u_int32_t clientId);
		void setMaxPlayers(int value);
		void setMinPlayers(int value);

		//TODO: Run one instance of a game for each room in a separate thread
};
