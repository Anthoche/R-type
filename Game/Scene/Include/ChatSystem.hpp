/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ChatSystem
*/

#pragma once

#include "../../../Engine/Rendering/Raylib.hpp"
#include <deque>
#include <optional>
#include <string>

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

	/**
	 * @brief Update the player username used when submitting messages.
	 */
	void setUsername(std::string username);

	/**
	 * @brief Append a printable codepoint to the input buffer.
	 */
	void appendCharacter(int codepoint);

	/**
	 * @brief Remove the last character from the input buffer (if any).
	 */
	void removeLastCharacter();

	/**
	 * @brief Submit the current input buffer as a chat message.
	 *
	 * The buffer is cleared after submission. Empty buffers are ignored.
	 */
	[[nodiscard]] std::optional<std::string> submitMessage();

	/**
	 * @brief Clear the current input buffer without submitting.
	 */
	void clearInput();

	/**
	 * @brief Add a message to the chat history (typically received from the network).
	 */
	void addMessage(const std::string &sender, const std::string &message);

	/**
	 * @brief Access the current list of messages.
	 */
	const std::deque<std::string> &messages() const { return _messages; }

	/**
	 * @brief Access the current input buffer.
	 */
	const std::string &inputBuffer() const { return _inputBuffer; }

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
	float _cursorTimer{0.f};
	std::string _username;
	std::string _inputBuffer;
	std::deque<std::string> _messages;
	std::size_t _maxMessages{6};
	int _fontSize{18};
	float _lineSpacing{4.f};
};
