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
				float spacing, Color color, Color textColor, bool isHovered, bool isClicked, bool isEnabled) {
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
	raylib.drawRectangleRounded(rect, 0.5, 10, color);
	raylib.drawTextEx(font, content, textPos, fontSize, spacing, textColor);
}

int getButtonsCenterY(int screenHeight, int numberOfButtons, int buttonHeight, int buttonSpacing) {
	int totalHeight = (numberOfButtons * buttonHeight) + ((numberOfButtons - 1) * buttonSpacing);
	return getElementCenter(screenHeight, totalHeight);
}
