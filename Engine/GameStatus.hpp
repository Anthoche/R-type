/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** GameStatus
*/

#ifndef RTYPE_GAMESTATUS_HPP
#define RTYPE_GAMESTATUS_HPP

/**
 * @brief The status of the game: WAITING, RUNNING, FINISHED
 */
enum class GameStatus {
	WAITING, ///< Waiting for players to join
	RUNNING, ///< Game is running
	FINISHED ///< Game is finished
};

#endif //RTYPE_GAMESTATUS_HPP
