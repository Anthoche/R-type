/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RenderUtils
*/

#include "RenderUtils.hpp"

float getElementCenter(int screenAxis, int elementAxis) {
	return (screenAxis / 2) - (elementAxis / 2);
}

void drawButton(Raylib &raylib, Vector2 position, Vector2 size, std::string const &content, Font font, int fontSize,
				float spacing, Color color, Color textColor, bool isHovered, bool isClicked, bool isEnabled, float roundness) {
	Vector2 textPos = {position.x + size.x / 2, position.y + size.y / 2};
	Vector2 textSize = raylib.measureTextEx(font, content, fontSize, spacing);
	textPos.x -= textSize.x / 2;
	textPos.y -= textSize.y / 2;

	Rectangle rect = {position.x, position.y, size.x, size.y};

	if (isEnabled) {
		if (isHovered) {
			Color temp = textColor;
			textColor = color;
			color = temp;
		}
		if (isClicked) {
			color.a -= 50;
		}
	} else {
		color.a = 100;
		textColor.a = 100;
	}
	raylib.drawRectangleRounded(rect, roundness, 10, color);
	raylib.drawTextEx(font, content, textPos, fontSize, spacing, textColor);
}

int getButtonsCenterY(int screenHeight, int numberOfButtons, int buttonHeight, int buttonSpacing) {
	int totalHeight = (numberOfButtons * buttonHeight) + ((numberOfButtons - 1) * buttonSpacing);
	return getElementCenter(screenHeight, totalHeight);
}

Vector2 getRealPos(Raylib &raylib, DynamicPosition pos, Vector2 offset, Vector2 margin, Vector2 size) {
	Vector2 finalPos = {0.f, 0.f};
	float bottomPosY = raylib.getRenderHeight() - margin.y - size.y;
	float centerPosX = getElementCenter(raylib.getRenderWidth(), size.x);
	float rightPosX = raylib.getRenderWidth() - size.x - margin.x;

	switch (pos) {
		case TOP_LEFT:
			finalPos = {margin.x, margin.y};
			break;
		case TOP_CENTER:
			finalPos = {centerPosX, margin.y};
			break;
		case TOP_RIGHT:
			finalPos = {rightPosX, margin.y};
			break;
		case BOTTOM_LEFT:
			finalPos = {margin.x, bottomPosY};
			break;
		case BOTTOM_CENTER:
			finalPos = {centerPosX, bottomPosY};
			break;
		case BOTTOM_RIGHT:
			finalPos = {rightPosX, bottomPosY};
			break;
	}
	finalPos.x += offset.x;
	finalPos.y += offset.y;
	return finalPos;
}

Vector2 getTextPos(Raylib &raylib, DynamicPosition pos, Vector2 offset, Vector2 margin, std::string const &content, Font font, float fontSize,
					float spacing) {
	Vector2 textSize = raylib.measureTextEx(font, content, fontSize, spacing);

	return getRealPos(raylib, pos, offset, margin, textSize);
}
