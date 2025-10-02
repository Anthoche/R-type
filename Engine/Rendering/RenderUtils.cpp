/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RenderUtils
*/

#include "RenderUtils.hpp"

int getCenterY(int screenHeight, int elementHeight) {
	return (screenHeight / 2) - (elementHeight / 2);
}

int getButtonsCenterY(int screenHeight, int numberOfButtons, int buttonHeight, int buttonSpacing) {
	int totalHeight = (numberOfButtons * buttonHeight) + ((numberOfButtons - 1) * buttonSpacing);
	return getCenterY(screenHeight, totalHeight);
}
