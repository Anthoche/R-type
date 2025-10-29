/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** sound entity
*/

#pragma once

#include "components.hpp"
#include "../../Include/registry.hpp"

namespace game::entities {

    /**
     * @brief Creates a sound entity for audio playback.
     *
     * Sound entities are used to play audio effects or background music in the game.
     * They can be configured to loop, play automatically, and have adjustable volume.
     *
     * @param reg Reference to the ECS registry where the entity will be registered.
     * @param soundPath Path to the audio file to be played.
     * @param volume Volume level of the sound (0.0 to 1.0, default: 1.0).
     * @param loop Whether the sound should loop continuously (default: false).
     * @param autoplay Whether the sound should start playing immediately when created (default: true).
     * @return The unique identifier of the created sound entity.
     */
    ecs::entity_t create_sound(ecs::registry &reg, const std::string &soundPath, float volume = 1.f, bool loop = false, bool autoplay = true);

}
