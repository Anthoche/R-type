/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ChatSystem
*/

#pragma once

#include "../../../Engine/Rendering/Raylib.hpp"

/**
 * @class ChatSystem
 * @brief Lightweight manager for the in-game chat overlay shell.
 *
 * Handles drawing the transparent chat panel and keeping track of the
 * "focus" state (when the player wants to type). Actual text handling
 * will be implemented in later steps.
 */
class ChatSystem {
public:
	/**
	 * @brief Construct the chat system.
	 * @param raylib Reference to the shared Raylib wrapper.
	 */
	explicit ChatSystem(Raylib &raylib);

	/**
	 * @brief Initialise runtime values that depend on render size.
	 */
	void init();

	/**
	 * @brief Update animated values (focus blend).
	 * @param deltaTime Time elapsed since last frame in seconds.
	 */
	void update(float deltaTime);

	/**
	 * @brief Render the chat panel.
	 */
	void render();

	/**
	 * @brief Toggle the focus state (called from input handling).
	 */
	void toggleFocus();

	/**
	 * @brief Get the current focus state.
	 * @return true if the chat is focused.
	 */
	[[nodiscard]] bool isFocused() const { return _isFocused; }

private:
	/**
	 * @brief Recalculate panel position/size when the window changes.
	 */
	void updatePanelGeometry();

	Raylib &_raylib;
	Rectangle _panelBounds{};
	bool _isFocused{false};
	float _focusBlend{0.f};
	float _margin{24.f};
};