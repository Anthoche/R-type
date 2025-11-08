/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** Shared UI theme colors
*/

#pragma once

#include <raylib.h>

namespace ui::theme {
	inline constexpr Color BackgroundTop{5, 5, 5, 255};
	inline constexpr Color BackgroundBottom{0, 31, 63, 255};
	inline constexpr Color Accent{10, 34, 64, 255};
	inline constexpr Color AccentText{255, 255, 255, 255};
	inline constexpr Color Secondary{15, 45, 90, 255};
	inline constexpr Color SecondaryText{255, 255, 255, 255};
	inline constexpr Color Neutral{20, 60, 110, 255};
	inline constexpr Color NeutralText{255, 255, 255, 255};
	inline constexpr Color Warning{228, 87, 87, 255};
	inline constexpr Color ButtonOutline{255, 255, 255, 180};
}
