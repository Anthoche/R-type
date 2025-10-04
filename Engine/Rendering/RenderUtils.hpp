/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RenderUtils
*/

#ifndef RTYPE_RENDERUTILS_HPP
#define RTYPE_RENDERUTILS_HPP

#include "Raylib.hpp"
#include <string>

float getElementCenter(int screenAxis, int elementAxis);

void drawButton(Raylib &raylib, Vector2 position, Vector2 size, std::string const &content, Font font, int fontSize, float spacing,
				Color color, Color textColor, bool isHovered, bool isClicked, bool isEnabled);

int getButtonsCenterY(int screenHeight, int numberOfButtons, int buttonHeight, int buttonSpacing);

#endif //RTYPE_RENDERUTILS_HPP
