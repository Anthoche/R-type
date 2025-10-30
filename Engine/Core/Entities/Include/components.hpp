/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** component
*/

#pragma once

#include <raylib.h>

#include "../../Include/entity.hpp"
#include "DynamicPosition.hpp"
#include <string>
#include <cstdint>


/**
 * @namespace component
 * @brief Contains all ECS components used throughout the game engine.
 */
namespace component {

    // =========================
    // POSITION / TRANSFORM
    // =========================

    /**
     * @struct position
     * @brief Represents the position of an entity in 3D world space.
     */
    struct position {
        float x{0.f}; ///< Horizontal position (X-axis).
        float y{0.f}; ///< Vertical position (Y-axis).
        float z{0.f}; ///< Depth position (Z-axis) for 3D positioning.
    };

    /**
     * @struct previous_position
     * @brief Stores the entity's position from the previous frame.
     *
     * Used for physics calculations, collision resolution, and interpolation.
     */
    struct previous_position {
        float x{0.f}; ///< Previous horizontal position.
        float y{0.f}; ///< Previous vertical position.
        float z{0.f}; ///< Previous depth position.
    };

    /**
     * @struct velocity
     * @brief Represents the movement speed of an entity in 3D space.
     */
    struct velocity {
        float vx{0.f}; ///< Velocity along the X-axis (units per second).
        float vy{0.f}; ///< Velocity along the Y-axis (units per second).
        float vz{0.f}; ///< Velocity along the Z-axis (units per second).
    };

    /**
     * @struct rotation
     * @brief Represents the orientation of an entity using Euler angles.
     *
     * All angles are specified in degrees.
     */
    struct rotation {
        float pitch{0.f}; ///< Rotation around the X-axis (looking up/down).
        float yaw{0.f};   ///< Rotation around the Y-axis (looking left/right).
        float roll{0.f};  ///< Rotation around the Z-axis (tilting).
    };

    /**
     * @struct scale
     * @brief Defines the scale/size multiplier for an entity in 3D space.
     */
    struct scale {
        float x{1.f}; ///< Scale factor along the X-axis.
        float y{1.f}; ///< Scale factor along the Y-axis.
        float z{1.f}; ///< Scale factor along the Z-axis.
    };

    /**
     * @struct dynamic_position
     * @brief Position that can be dynamically calculated or animated.
     *
     * Useful for entities that follow paths or have complex movement patterns.
     */
    struct dynamic_position {
        DynamicPosition position; ///< Dynamic position calculator.
        float offsetX{0.f}; ///< Additional offset from the calculated position (X-axis).
        float offsetY{0.f}; ///< Additional offset from the calculated position (Y-axis).
        float offsetZ{0.f}; ///< Additional offset from the calculated position (Z-axis).
    };

    // =========================
    // RENDERABLE / VISUAL
    // =========================

    /**
     * @struct drawable
     * @brief Defines the visual properties and dimensions of a renderable entity.
     *
     * Can represent both 2D rectangles and 3D boxes.
     */
    struct drawable {
        float width{32.f};  ///< Width of the drawable (X dimension).
        float height{32.f}; ///< Height of the drawable (Y dimension).
        float depth{32.f};  ///< Depth of the drawable (Z dimension) for 3D objects.
        Color color{WHITE}; ///< Base color of the entity.
    };

    /**
     * @struct sprite
     * @brief 2D sprite texture component for rendering textured entities.
     */
    struct sprite {
        std::string image_path{}; ///< Path to the sprite image file.
        Texture2D texture{};      ///< Loaded texture data (managed by Raylib).
        float scale{1.f};         ///< Scale multiplier for the sprite.
        float rotation{0.f};      ///< Rotation angle in degrees.
    };

    /**
     * @struct pattern_element
     * @brief Identifies an entity as part of a named pattern or formation.
     */
    struct pattern_element {
        std::string pattern_name{}; ///< Name of the pattern this element belongs to.
    };

    /**
     * @struct model3D
     * @brief 3D model component for rendering 3D meshes.
     */
    struct model3D {
        std::string model_path{}; ///< Path to the 3D model file.
        Model model{};            ///< Loaded 3D model data (managed by Raylib).
        float scale{1.f};         ///< Uniform scale multiplier for the model.
        rotation rotation_angles{}; ///< Rotation of the model in 3D space.
    };

    // =========================
    // AUDIO
    // =========================

    /**
     * @struct audio
     * @brief Audio component for entities that emit sound.
     */
    struct audio {
        std::string sound_path{}; ///< Path to the audio file.
        float volume{1.f};        ///< Volume level (0.0 to 1.0).
        bool loop{false};         ///< Whether the sound should loop continuously.
        bool autoplay{false};     ///< Whether the sound plays automatically when loaded.
    };

    // =========================
    // TEXT / UI
    // =========================

    /**
     * @struct text
     * @brief Text rendering component for displaying strings on screen.
     */
    struct text {
        std::string content{};  ///< The text string to display.
        int font_size{24};      ///< Size of the font in pixels.
        float spacing{1.f};     ///< Spacing between characters.
        Color color{WHITE};     ///< Color of the text.
        Font font{};            ///< Custom font (uses default if not set).
    };

    /**
     * @struct font
     * @brief Custom font resource for text rendering.
     */
    struct font {
        std::string font_path{}; ///< Path to the font file (TTF, OTF, etc.).
    };

    /**
     * @struct clickable
     * @brief Makes an entity interactive with mouse clicks.
     */
    struct clickable {
        std::string id{};       ///< Unique identifier for the clickable element.
        bool enabled{true};     ///< Whether clicking is currently enabled.
        bool isClicked{false};  ///< True on the frame when clicked.
    };

    /**
     * @struct hoverable
     * @brief Detects when the mouse cursor is over an entity.
     */
    struct hoverable {
        std::string id{};       ///< Unique identifier for the hoverable element.
        bool isHovered{false};  ///< True when the mouse is over this entity.
    };

    // =========================
    // INPUT / CONTROL
    // =========================

    /**
     * @struct controllable
     * @brief Marks an entity as player-controllable with input.
     */
    struct controllable {
        float speed{200.f};  ///< Movement speed in units per second.
        bool can_jump{false}; ///< Whether the entity can jump.
        bool can_fly{false};  ///< Whether the entity can fly/move in all directions.
    };

    // =========================
    // COMBAT / STATS
    // =========================

    /**
     * @struct health
     * @brief Health/life points component for entities that can take damage.
     */
    struct health {
        int current{100}; ///< Current health value.
        int max{100};     ///< Maximum possible health.
    };

    /**
     * @struct damage
     * @brief Damage component for entities that can harm others.
     */
    struct damage {
        int amount{10}; ///< Amount of damage dealt on contact or hit.
    };

    // =========================
    // COLLISION / PHYSICS
    // =========================

    /**
     * @struct collision_box
     * @brief Defines the collision boundaries for an entity.
     */
    struct collision_box {
        float width{32.f};  ///< Width of the collision box.
        float height{32.f}; ///< Height of the collision box.
        float depth{32.f};  ///< Depth of the collision box for 3D collision.
    };

    /**
     * @struct hitbox_link
     * @brief Links a hitbox entity to its parent owner entity.
     *
     * Allows hitboxes to be separate entities that follow their owner.
     */
    struct hitbox_link {
        ecs::entity_t owner{0}; ///< Entity ID of the owner.
        float offsetX{0.f};     ///< X offset from the owner's position.
        float offsetY{0.f};     ///< Y offset from the owner's position.
        float offsetZ{0.f};     ///< Z offset from the owner's position.
    };

    // =========================
    // GAME LOGIC / TYPES
    // =========================

    /**
     * @enum entity_type
     * @brief Enumeration of all entity types in the game.
     *
     * Used for categorizing and identifying different kinds of entities.
     */
    enum class entity_type {
        PLAYER,          ///< Player-controlled character.
        ENEMY,           ///< Hostile enemy entity.
        OBSTACLE,        ///< Static obstacle or wall.
        POWERUP,         ///< Collectible power-up item.
        ITEM,            ///< Generic item entity.
        TRAP,            ///< Harmful trap entity.
        GATE,            ///< Door or gate entity.
        WEAPON,          ///< Weapon entity.
        PLATFORM,        ///< Moving or static platform.
        DECORATION,      ///< Non-interactive visual decoration.
        CHECKPOINT,      ///< Progress checkpoint or save point.
        SPAWNER,         ///< Entity that spawns other entities.
        TRIGGERZONE,     ///< Invisible zone that triggers events.
        BACKGROUND,      ///< Background visual element.
        SOUND,           ///< Audio source entity.
        TEXT,            ///< Text display entity.
        BUTTON,          ///< Interactive button UI element.
        PROJECTILE,      ///< Projectile or bullet entity.
        IMAGE,           ///< Static image entity.
        RANDOM_ELEMENT,  ///< Randomly generated element.
        PNG,             ///< PNG image entity.
        PATTERN_ELEMENT  ///< Element part of a pattern or formation.
    };

    /**
     * @struct type
     * @brief Assigns a type classification to an entity.
     */
    struct type {
        entity_type value{entity_type::PLAYER}; ///< The entity's type classification.
    };

    // =========================
    // NETWORK / CLIENT
    // =========================

    /**
     * @struct client_id
     * @brief Network identifier for multiplayer entities.
     *
     * Associates an entity with a specific network client.
     */
    struct client_id {
        uint32_t id{0}; ///< Unique client identifier assigned by the server.
    };

}