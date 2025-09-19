/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Game
*/

#ifndef RTYPE_GAME_HPP
	#define RTYPE_GAME_HPP

	#include <string>
	#include <thread>
	#include "../Client/Network_client/Include/client.hpp"
#include "../Engine/Rendering/scene/SceneHandler.hpp"

class Game {
	public:
		Game(const std::string &serverIp, const std::string &clientName);
		~Game() = default;

		void run();

	private:
		GameClient _client;
		std::thread _networkThread;
		SceneHandler _sceneHandler;
};


#endif //RTYPE_GAME_HPP
