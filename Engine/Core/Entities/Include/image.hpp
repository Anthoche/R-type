/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** image
*/

#ifndef RTYPE_IMAGE_HPP
#define RTYPE_IMAGE_HPP

#include "components.hpp"
#include "registry.hpp"

namespace game::entities {
	/**
	 * Create an image entity
	 */
	ecs::entity_t create_image(ecs::registry &reg, Texture2D texture, Vector2 pos, Vector2 size);

	/**
	* Create an image at a dynamic position
	*/
	ecs::entity_t create_image(ecs::registry &reg, Texture2D texture, DynamicPosition pos, Vector2 offset);
}

#endif //RTYPE_IMAGE_HPP
