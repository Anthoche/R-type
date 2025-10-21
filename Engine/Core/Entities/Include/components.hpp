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
 * @brief Contains all ECS components for the game.
 */
namespace component {

    // =========================
    // POSITION / TRANSFORM
    // =========================

    /**
     * @struct position
     * @brief Represents the position of an entity in 3D space.
     */
    struct position {
        float x{0.f};
        float y{0.f};
        float z{0.f}; ///< Added Z for 3D positioning
    };

    /**
     * @struct previous_position
     * @brief Stores previous frame position for physics/collision resolution in 3D.
     */
    struct previous_position {
        float x{0.f};
        float y{0.f};
        float z{0.f};
    };

    /**
     * @struct velocity
     * @brief Represents velocity in 3D space.
     */
    struct velocity {
        float vx{0.f};
        float vy{0.f};
        float vz{0.f};
    };

    /**
     * @struct rotation
     * @brief Represents rotation of an entity in 3D space (Euler angles in degrees).
     */
    struct rotation {
        float pitch{0.f}; ///< Rotation around X
        float yaw{0.f};   ///< Rotation around Y
        float roll{0.f};  ///< Rotation around Z
    };

    /**
     * @struct scale
     * @brief Scale factor for an entity in 3D.
     */
    struct scale {
        float x{1.f};
        float y{1.f};
        float z{1.f};
    };

    struct dynamic_position {
        DynamicPosition position;
        float offsetX{0.f};
        float offsetY{0.f};
        float offsetZ{0.f};

    };

    // =========================
    // RENDERABLE / VISUAL
    // =========================

    /**
     * @struct drawable
     * @brief 2D or 3D renderable entity (size, color).
     */
    struct drawable {
        float width{32.f};
        float height{32.f};
        float depth{32.f}; ///< For 3D objects
        Color color{WHITE};
    };

    /**
     * @struct sprite
     * @brief Optional 2D sprite component.
     */
    struct sprite {
        std::string image_path{};
        Texture2D texture{};
        float scale{1.f};
        float rotation{0.f};
    };

    /**
     * @struct model3D
     * @brief Optional 3D model for the entity.
     */
    struct model3D {
        std::string model_path{};
        Model model{};
        float scale{1.f};
        rotation rotation_angles{};
    };

    // =========================
    // AUDIO
    // =========================

    struct audio {
        std::string sound_path{};
        float volume{1.f};
        bool loop{false};
        bool autoplay{false};
    };

    // =========================
    // TEXT / UI
    // =========================

    struct text {
        std::string content{};
        int font_size{24};
        float spacing{1.f};
        Color color{WHITE};
        Font font{};
    };

    struct font {
        std::string font_path{};
    };

    struct clickable {
        std::string id{};
        bool enabled{true};
        bool isClicked{false};
    };

    struct hoverable {
        std::string id{};
        bool isHovered{false};
    };

    // =========================
    // INPUT / CONTROL
    // =========================

    struct controllable {
        float speed{200.f};
        bool can_jump{false};
        bool can_fly{false};
    };

    // =========================
    // COMBAT / STATS
    // =========================

    struct health {
        int current{100};
        int max{100};
    };

    struct damage {
        int amount{10};
    };

    // =========================
    // COLLISION / PHYSICS
    // =========================

    struct collision_box {
        float width{32.f};
        float height{32.f};
        float depth{32.f};
    };

    struct hitbox_link {
        ecs::entity_t owner{0};
        float offsetX{0.f};
        float offsetY{0.f};
        float offsetZ{0.f};
    };

    // =========================
    // GAME LOGIC / TYPES
    // =========================

    enum class entity_type {
        PLAYER,
        ENEMY,
        OBSTACLE,
        POWERUP,
        ITEM,
        TRAP,
        GATE,
        WEAPON,
        PLATFORM,
        DECORATION,
        CHECKPOINT,
        SPAWNER,
        TRIGGERZONE,
        BACKGROUND,
        SOUND,
        TEXT,
        BUTTON,
        PROJECTILE,
        IMAGE,
        RANDOM_ELEMENT,
        PNG
    };

    struct type {
        entity_type value{entity_type::PLAYER};
    };

    // =========================
    // NETWORK / CLIENT
    // =========================

    struct client_id {
        uint32_t id{0};
    };

}
