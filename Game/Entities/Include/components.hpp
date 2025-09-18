/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** component
*/

#pragma once

namespace component {
	// Composants de base pour le jeu R-Type

	// Position dans l'espace 2D
	struct position {
		float x{0.f};
		float y{0.f};
	};

	// Vitesse et direction de mouvement
	struct velocity {
		float vx{0.f};
		float vy{0.f};
	};

	// Élément graphique affichable
	struct drawable {
		float width{32.f};
		float height{32.f};
		float r{1.f}, g{1.f}, b{1.f}, a{1.f}; // Couleur RGBA
	};

	// Entité contrôlable par le joueur
	struct controllable {
		float speed{200.f};
	};

	// Santé de l'entité
	struct health {
		int current{100};
		int max{100};
	};

	// Dégâts infligés par l'entité
	struct damage {
		int amount{10};
	};

	// Type d'entité pour la logique de jeu
	enum class entity_type {
		PLAYER,
		ENEMY,
		BULLET,
		POWERUP,
		OBSTACLE
	};

	struct type {
		entity_type value{entity_type::PLAYER};
	};

	// Collision box pour la détection de collision
	struct collision_box {
		float width{32.f};
		float height{32.f};
	};
}
