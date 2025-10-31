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
#include <map>
#include <cstdint>
#include "../Include/IServerGame.hpp"
#include "GameStatus.hpp"

class IServerGame;
class Connexion;

/**
 * @class Room
 * @brief Represents a multiplayer game room on the server.
 *
 * A Room manages a group of players waiting to play or actively playing together.
 * It tracks player connections, ready states, game configuration, and coordinates
 * the lifecycle of the associated game instance. Each room has a minimum and maximum
 * player count and runs its own independent game server thread when the game starts.
 */
class Room {
	private:
		Connexion &_connexion; ///< Reference to the network connection manager.
		int _maxPlayers; ///< Maximum number of players allowed in this room.
		int _minPlayers; ///< Minimum number of players required to start the game.
		std::string _gameName; ///< Display name for this game room.
		GameStatus _gameStatus; ///< Current status of the room (waiting, ready, playing, etc.).
		std::shared_ptr<IServerGame> _game; ///< Pointer to the active game instance.
		bool _isGameServerStarted; ///< Flag indicating if the game server thread has started.
		std::map<uint32_t, bool> _clients; ///< Maps client IDs to their ready status (true = ready).
		int _roomHost; ///< Client ID of the room host/creator (-1 if no host).

	public:
		/**
		 * @brief Constructs a new game room.
		 *
		 * @param connexion Reference to the server's connection manager.
		 * @param maxPlayers Maximum number of players allowed in the room.
		 * @param minPlayers Minimum number of players required to start.
		 * @param gameName Display name for the room.
		 * @param _roomHost Client ID of the room creator (default: -1 for no host).
		 */
		Room(Connexion &connexion, int maxPlayers, int minPlayers, std::string const &gameName, int _roomHost = -1);

		/**
		 * @brief Destructor that cleans up room resources.
		 */
		~Room() = default;

		/**
		 * @brief Gets the maximum number of players allowed in this room.
		 *
		 * @return Maximum player count.
		 */
		int getMaxPlayers() const;

		/**
		 * @brief Gets the minimum number of players required to start the game.
		 *
		 * @return Minimum player count.
		 */
		int getMinPlayers() const;

		/**
		 * @brief Gets the current status of the room.
		 *
		 * @return Current GameStatus (waiting, ready, playing, etc.).
		 */
		GameStatus getGameStatus() const;

		/**
		 * @brief Gets the game instance associated with this room.
		 *
		 * @return Shared pointer to the IServerGame instance, or nullptr if not started.
		 */
		std::shared_ptr<IServerGame> getGame();

		/**
		 * @brief Gets the display name of this room.
		 *
		 * @return Room name as a string.
		 */
		std::string getGameName() const;

		/**
		 * @brief Gets the map of clients and their ready status.
		 *
		 * @return Map of client IDs to boolean ready flags.
		 */
		std::map<uint32_t, bool> getClients() const;

		/**
		 * @brief Checks if the room is at maximum capacity.
		 *
		 * @return true if the room has reached maximum players, false otherwise.
		 */
		bool isFull() const;

		/**
		 * @brief Checks if the room has no players.
		 *
		 * @return true if the room is empty, false otherwise.
		 */
		bool isEmpty() const;

		/**
		 * @brief Checks if all players in the room are ready to start.
		 *
		 * @return true if minimum players met and all players confirmed ready, false otherwise.
		 */
		bool isReady() const;

		/**
		 * @brief Checks if a specific client is in this room.
		 *
		 * @param clientId The client ID to check.
		 * @return true if the client is a member of this room, false otherwise.
		 */
		bool isClientInRoom(uint32_t clientId) const;

		/**
		 * @brief Gets the client ID of the room host.
		 *
		 * @return Client ID of the room creator, or -1 if no host.
		 */
		int getRoomHost() const;

		/**
		 * @brief Checks if the game server has been started for this room.
		 *
		 * @return true if the game instance is running, false otherwise.
		 */
		bool isGameServerStarted() const;

		/**
		 * @brief Marks a client as ready to start the game.
		 *
		 * Updates the client's ready status. If all players become ready,
		 * the game may automatically start.
		 *
		 * @param clientId The client ID to mark as ready.
		 */
		void setClientConfirmed(uint32_t clientId);

		/**
		 * @brief Adds a new client to the room.
		 *
		 * Registers a player as a member of this room. Initially marked as not ready.
		 *
		 * @param clientId The client ID to add.
		 */
		void addClient(uint32_t clientId);

		/**
		 * @brief Removes a client from the room.
		 *
		 * Unregisters a player from this room, freeing up a slot.
		 *
		 * @param clientId The client ID to remove.
		 */
		void removeClient(uint32_t clientId);

		/**
		 * @brief Sets the maximum number of players allowed.
		 *
		 * @param value New maximum player count.
		 */
		void setMaxPlayers(int value);

		/**
		 * @brief Sets the minimum number of players required to start.
		 *
		 * @param value New minimum player count.
		 */
		void setMinPlayers(int value);

		/**
		 * @brief Sets the current game status.
		 *
		 * @param status New GameStatus to apply.
		 */
		void setGameStatus(GameStatus status);

		/**
		 * @brief Starts the game instance for this room.
		 *
		 * Initializes the game server, creates a new game instance, and launches
		 * it in a separate thread. Players transition from the lobby to active gameplay.
		 *
		 * @param roomId The unique identifier for this room.
		 */
		void startGame(int roomId);
};