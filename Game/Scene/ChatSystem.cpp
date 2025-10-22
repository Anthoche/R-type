/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** ChatSystem implementation
*/

#include "Include/ChatSystem.hpp"
#include <algorithm>

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
}

void ChatSystem::update(float deltaTime)
{
	const float target = _isFocused ? 1.f : 0.f;
	const float speed = 8.f;
	const float step = std::clamp(deltaTime * speed, 0.f, 1.f);
	_focusBlend += (target - _focusBlend) * step;
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