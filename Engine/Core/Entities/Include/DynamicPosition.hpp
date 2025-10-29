/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** DynamicPosition
*/

#pragma once

	/**
	 * @enum DynamicPosition
	 * @brief Defines anchor positions for dynamic UI element placement.
	 *
	 * This enumeration is used to position entities relative to screen edges or corners,
	 * allowing for responsive layouts that adapt to different screen sizes.
	 */
	enum DynamicPosition {
		TOP_LEFT,      ///< Anchored to the top-left corner of the screen.
		TOP_CENTER,    ///< Anchored to the top-center of the screen.
		TOP_RIGHT,     ///< Anchored to the top-right corner of the screen.
		BOTTOM_LEFT,   ///< Anchored to the bottom-left corner of the screen.
		BOTTOM_CENTER, ///< Anchored to the bottom-center of the screen.
		BOTTOM_RIGHT   ///< Anchored to the bottom-right corner of the screen.
};
