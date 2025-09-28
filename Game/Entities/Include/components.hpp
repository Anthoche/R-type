/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** component
*/

#pragma once

#include <raylib.h>

#include "../../Engine/Core/Include/entity.hpp"
#include <string>

/**
 * @namespace component
 * @brief Contains all ECS components used in the R-Type game.
 */
namespace component {
	/**
	* @struct position
	* @brief Represents the position of an entity in 2D space.
	*/
	struct position {
		float x{0.f}; ///< X coordinate
		float y{0.f}; ///< Y coordinate
	};

	/**
	* @struct previous_position
	* @brief Stores previous frame position for collision resolution.
	*/
	struct previous_position {
		float x{0.f};
		float y{0.f};
	};

	/**
	* @struct velocity
	* @brief Represents the velocity of an entity in 2D space.
	*/
	struct velocity {
		float vx{0.f}; ///< Velocity along X
		float vy{0.f}; ///< Velocity along Y
	};

	/**
	* @struct drawable
	* @brief Represents a renderable entity with size and color.
	*/
	struct drawable {
		float width{32.f}; ///< Width of the entity
		float height{32.f}; ///< Height of the entity
		float r{1.f}, g{1.f}, b{1.f}, a{1.f}; ///< Color in RGBA
	};

	/**
	* @struct sprite
	* @brief Optional sprite to render an image for an entity.
	*/
	struct sprite {
		std::string image_path{}; ///< Filepath to the image/texture
		float scale{1.f}; ///< Uniform scale for rendering
		float rotation{0.f}; ///< Rotation in degrees
	};

	/**
	* @struct audio
	* @brief Optional audio component to attach a sound/music to an entity.
	*/
	struct audio {
		std::string sound_path{}; ///< Filepath to the sound/music
		float volume{1.f}; ///< Playback volume [0..1]
		bool loop{false}; ///< Should the audio loop
		bool autoplay{false}; ///< Start playing on creation
	};

	/**
	* @struct text
	* @brief Text content and styling for on-screen labels.
	*/
	struct text {
		std::string content{}; ///< UTF-8 text to display
		int font_size{24}; ///< Font size in pixels
		unsigned char r{255}, g{255}, b{255}, a{255}; ///< RGBA color
		float spacing{1.f}; ///< Glyph spacing in pixels
	};

	/**
	* @struct font
	* @brief Optional font resource for text rendering.
	*/
	struct font {
		std::string font_path{}; ///< Filepath to TTF/OTF font
	};

	/**
	* @struct clickable
	* @brief Marks an entity as clickable
	*/
	struct clickable {
		std::string id{};
		bool isClicked{false};
	};

	struct hoverable {
		std::string id{};
		bool isHovered{false};
	};

	/**
	* @struct controllable
	* @brief Marks an entity as player-controllable.
	*/
	struct controllable {
		float speed{200.f}; ///< Movement speed
	};

	/**
	* @struct health
	* @brief Represents health points of an entity.
	*/
	struct health {
		int current{100}; ///< Current health
		int max{100}; ///< Maximum health
	};

	/**
	* @struct damage
	* @brief Represents damage dealt by an entity.
	*/
	struct damage {
		int amount{10}; ///< Damage amount
	};

	/**
	* @enum entity_type
	* @brief Defines the type of an entity for game logic.
	*/
	enum class entity_type {
		PLAYER, ///< Player entity
		ENEMY, ///< Enemy entity
		BULLET, ///< Projectile
		POWERUP, ///< Power-up item
		OBSTACLE, ///< Obstacle in the game
		BACKGROUND, ///< Background entity
		SOUND, ///< Sound-only entity
		TEXT, ///< Text label entity
		BUTTON ///< Button entity
	};

	/**
	* @struct type
	* @brief Component to store the entity type.
	*/
	struct type {
		entity_type value{entity_type::PLAYER}; ///< Type of the entity
	};

	/**
	* @struct collision_box
	* @brief Defines the collision bounds of an entity.
	*/
	struct collision_box {
		float width{32.f}; ///< Width of the collision box
		float height{32.f}; ///< Height of the collision box
	};

	/**
	* @struct hitbox_link
	* @brief Associates a hitbox entity to its owner entity with an optional offset.
	*/
	struct hitbox_link {
		ecs::entity_t owner{0}; ///< Owner entity that this hitbox follows
		float offsetX{0.f}; ///< X offset relative to owner position
		float offsetY{0.f}; ///< Y offset relative to owner position
	};
}
