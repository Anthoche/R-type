/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ChatSystem implementation
*/

#include "Include/ChatSystem.hpp"
#include <algorithm>
#include <cmath>
#include <format>

namespace {
	Color mixColor(Color a, Color b, float t)
	{
		t = std::clamp(t, 0.f, 1.f);
		auto lerpChannel = [t](unsigned char ca, unsigned char cb) -> unsigned char {
			return static_cast<unsigned char>(ca + (cb - ca) * t);
		};
		return {
			lerpChannel(a.r, b.r),
			lerpChannel(a.g, b.g),
			lerpChannel(a.b, b.b),
			lerpChannel(a.a, b.a)
		};
	}
}

ChatSystem::ChatSystem(Raylib &raylib) : _raylib(raylib)
{
	updatePanelGeometry();
}

void ChatSystem::init()
{
	updatePanelGeometry();
	_focusBlend = 0.f;
	_isFocused = false;
	_inputBuffer.clear();
	_messages.clear();
	_cursorTimer = 0.f;
}

void ChatSystem::update(float deltaTime)
{
	const float target = _isFocused ? 1.f : 0.f;
	const float speed = 8.f;
	const float step = std::clamp(deltaTime * speed, 0.f, 1.f);
	_focusBlend += (target - _focusBlend) * step;
	_cursorTimer += deltaTime;
	if (_cursorTimer > 1.0f)
		_cursorTimer -= 1.0f;
	updatePanelGeometry();
}

void ChatSystem::render()
{
	updatePanelGeometry();

	const Color baseColor = mixColor(
		Color{8, 8, 12, 150},
		Color{18, 18, 32, 200},
		_focusBlend
	);

	const Color glassSheen = mixColor(
		Color{40, 40, 52, 105},
		Color{80, 120, 200, 150},
		_focusBlend
	);

	const Color borderColor = mixColor(
		Color{80, 90, 110, 100},
		Color{130, 190, 255, 190},
		_focusBlend
	);

	_raylib.drawRectangleRounded(_panelBounds, 0.12f, 16, baseColor);

	const float sheenHeight = _panelBounds.height * 0.2f;
	Rectangle sheenBounds = {
		_panelBounds.x + 8.f,
		_panelBounds.y + _panelBounds.height - sheenHeight - 8.f,
		_panelBounds.width - 16.f,
		sheenHeight
	};
	if (sheenBounds.width > 0 && sheenBounds.height > 0) {
		_raylib.drawRectangleRounded(sheenBounds, 0.18f, 16, glassSheen);
	}

	const Color messageColor = Color{220, 228, 255, 230};
	const Color inputColorFocused = Color{240, 248, 255, 240};
	const Color inputColorIdle = Color{200, 208, 220, 180};
	const float padding = 14.f;
	const float inputPadding = 18.f;
	const float textStartX = _panelBounds.x + padding;
	const float textAreaTop = _panelBounds.y + padding;
	const float textAreaBottom = sheenBounds.y - padding;
	float lineY = textAreaTop;
	const float lineAdvance = static_cast<float>(_fontSize) + _lineSpacing;
	std::size_t maxLines = lineAdvance > 0.f ? static_cast<std::size_t>(std::floor((textAreaBottom - textAreaTop) / lineAdvance)) : 0;
	if (maxLines == 0)
		maxLines = 1;
	std::size_t startIndex = _messages.size() > maxLines ? _messages.size() - maxLines : 0;

	for (std::size_t i = startIndex; i < _messages.size(); ++i) {
		const auto &message = _messages[i];
		if (lineY + _fontSize > textAreaBottom)
			break;
		_raylib.drawText(message, static_cast<int>(textStartX), static_cast<int>(lineY), _fontSize, messageColor);
		lineY += lineAdvance;
	}

	std::string inputText;
	if (_inputBuffer.empty()) {
		if (_isFocused) {
			inputText = "> ";
		} else {
			inputText = "> Tape un message...";
		}
	} else {
		inputText = std::format("> {}", _inputBuffer);
	}

	bool caretVisible = _isFocused && _cursorTimer < 0.5f;
	if (_isFocused) {
		inputText += caretVisible ? "|" : " ";
	}

	const float inputTextY = sheenBounds.y + (sheenBounds.height - _fontSize) / 2.f - 2.f;
	const Color inputColor = _isFocused ? inputColorFocused : inputColorIdle;
	_raylib.drawText(inputText, static_cast<int>(_panelBounds.x + inputPadding), static_cast<int>(inputTextY), _fontSize, inputColor);

	_raylib.drawRectangleLines(
		static_cast<int>(_panelBounds.x),
		static_cast<int>(_panelBounds.y),
		static_cast<int>(_panelBounds.width),
		static_cast<int>(_panelBounds.height),
		borderColor
	);
}

void ChatSystem::toggleFocus()
{
	_isFocused = !_isFocused;
	if (_isFocused)
		_cursorTimer = 0.f;
}

void ChatSystem::updatePanelGeometry()
{
	const float renderWidth = static_cast<float>(_raylib.getRenderWidth());
	const float renderHeight = static_cast<float>(_raylib.getRenderHeight());

	const float maxWidth = 520.f;
	const float maxHeight = 220.f;

	const float width = std::clamp(renderWidth * 0.32f, 320.f, maxWidth);
	const float height = std::clamp(renderHeight * 0.24f, 160.f, maxHeight);

	_panelBounds.width = width;
	_panelBounds.height = height;
	_panelBounds.x = _margin;
	_panelBounds.y = renderHeight - height - _margin;
}

void ChatSystem::setUsername(std::string username)
{
	_username = std::move(username);
}

void ChatSystem::appendCharacter(int codepoint)
{
	if (codepoint <= 0)
		return;
	if (codepoint == 9) // tab
		return;

	if (codepoint >= 32 && codepoint <= 126) {
		if (_inputBuffer.size() < 160)
			_inputBuffer.push_back(static_cast<char>(codepoint));
	}
}

void ChatSystem::removeLastCharacter()
{
	if (!_inputBuffer.empty())
		_inputBuffer.pop_back();
}

void ChatSystem::submitMessage()
{
	if (_inputBuffer.empty())
		return;
	std::string name = _username.empty() ? "Player" : _username;
	std::string composed = std::format("{}: {}", name, _inputBuffer);
	_messages.push_back(std::move(composed));
	while (_messages.size() > _maxMessages)
		_messages.pop_front();
	_inputBuffer.clear();
}

void ChatSystem::clearInput()
{
	_inputBuffer.clear();
}