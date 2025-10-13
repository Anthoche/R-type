/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameStatus
*/

#ifndef RTYPE_GAMESTATUS_HPP
#define RTYPE_GAMESTATUS_HPP

/**
 * @brief The status of the game: WAITING_PLAYERS, PENDING_START, RUNNING, FINISHED
 */
enum class GameStatus {
	WAITING_PLAYERS, ///< Waiting for players to join
	PENDING_START, ///< Waiting for the game to start
	RUNNING, ///< Game is running
	FINISHED ///< Game is finished
};

#endif //RTYPE_GAMESTATUS_HPP
