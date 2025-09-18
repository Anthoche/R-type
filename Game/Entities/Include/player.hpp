/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** player
*/

#pragma once

#include "registry.hpp"
#include "entity.hpp"
#include "components.hpp"

namespace game::entities {

// Factory pour créer un joueur avec tous ses composants
ecs::entity_t create_player(ecs::registry &reg, float x = 100.f, float y = 300.f);

// Système de contrôle du joueur
void setup_player_control_system(ecs::registry &reg);

// Système de limitation de mouvement (garder le joueur dans l'écran)
void setup_player_bounds_system(ecs::registry &reg, float screen_width, float screen_height);

} // namespace game::entities
