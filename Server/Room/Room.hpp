/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Room
*/

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "../../Engine/GameStatus.hpp"

class Room {
	private:
		int _maxPlayers;
		int _minPlayers;
		std::string _gameName;
		std::vector<uint32_t> _clients;
		int _roomHost;

	public:
		Room(int maxPlayers, int minPlayers, std::string const &gameName);
		Room(int maxPlayers, int minPlayers, int _roomHost, std::string const &gameName);
		~Room() = default;

		int getMaxPlayers() const;
		int getMinPlayers() const;
		std::string getGameName() const;
		std::vector<uint32_t> getClients() const;
		bool isFull() const;
		bool isEmpty() const;
		bool isClientInRoom(uint32_t clientId) const;
		int getRoomHost() const;

		void addClient(uint32_t clientId);
		void removeClient(uint32_t clientId);
		void setMaxPlayers(int value);
		void setMinPlayers(int value);

		//TODO: Run one instance of a game for each room in a separate thread
};
