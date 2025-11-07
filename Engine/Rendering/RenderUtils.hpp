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
#include "DynamicPosition.hpp"

float getElementCenter(int screenAxis, int elementAxis);

void drawButton(Raylib &raylib, Vector2 position, Vector2 size, std::string const &content, Font font, int fontSize, float spacing,
				Color color, Color textColor, bool isHovered, bool isClicked, bool isEnabled, float roundness = 0.5f);

void drawSceneBackground(Raylib &raylib, Color topColor, Color bottomColor);

int getButtonsCenterY(int screenHeight, int numberOfButtons, int buttonHeight, int buttonSpacing);

/**
 * @brief Calculates the screen position for text based on dynamic positioning.
 * @param raylib Raylib instance
 * @param pos The dynamic position (e.g., TOP_LEFT, BOTTOM_CENTER).
 * @param offset Additional offset from the calculated position.
 * @param margin Margin to apply
 * @param content The text string to be rendered.
 * @param font The font of the text
 * @param fontSize The size of the font
 * @param spacing The letter spacing
 * @return A Vector2 containing the final screen coordinates for the text.
 */
Vector2 getTextPos(Raylib &raylib, DynamicPosition pos, Vector2 offset, Vector2 margin, std::string const &content, Font font,
					float fontSize, float spacing);

/**
 * @brief Calculates the screen position for an element based on dynamic positioning.
 * @param raylib Raylib instance
 * @param pos The dynamic position enum value.
 * @param offset Additional offset from the calculated position.
 * @param margin Margin to apply
 * @param size The width and height of the element.
 * @return A Vector2 containing the final screen coordinates.
 */
Vector2 getRealPos(Raylib &raylib, DynamicPosition pos, Vector2 offset, Vector2 margin, Vector2 size);

#endif //RTYPE_RENDERUTILS_HPP
