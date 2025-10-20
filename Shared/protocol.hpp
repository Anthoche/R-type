/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** protocol
*/


#pragma once

#include <cstdint>
#include <string>

/**
 * @brief Enum for all network message types exchanged between client and server.
 */
enum class MessageType : uint8_t {
	ClientHello, /**< Client greeting the server */
	ServerAssignId, /**< Server assigns an ID to the client */
	ClientRoomIdAsk, /**< Client asking for a connexion to a specific room */
	ServerRoomAssignId, /**< Assign room to client */
	ClientFetchRooms, /**< Fetch room list actions for client */
	ServerSendRooms, /**< Send rooms to client */
	GameStart, /**< Server notifies clients that the game is starting */
	ClientInput, /**< Client sends input for the current frame */
	StateUpdate, /**< Server sends updated state for a client */
	EnemySpawn, /**< Server spawns a new enemy */
	EnemyUpdate, /**< Server updates an enemy's position */
	EnemyDespawn, /**< Server removes an enemy */
	ObstacleSpawn, /**< Server spawns a new obstacle */
	ObstacleDespawn, /**< Server removes an obstacle */
	ClientShoot, /**< CLient send a shoot */
	ProjectileSpawn, /**< Server create a projectile */
	ProjectileUpdate, /**< Server update a projectile */
	ProjectileDespawn, /**< Server delete an projectile */
	EnemyProjectileSpawn, /**< Server spawns an enemy projectile */
	EnemyProjectileUpdate, /**< Server updates an enemy projectile */
	EnemyProjectileDespawn, /**< Server removes an enemy projectile */
	EntityData, /**< Server → Clients: synchronisation entité ECS */
	SceneState, /**< Client → Server: indique la scène courante */
	PlayerDeath, /**< Server notifies clients that a player has died */
	PlayerHealth, /**< Server updates a player's health */
	GlobalScore, /**< Server updates the global score */
	IndividualScore /**< Server updates a player's individual score */
};

/**
 * @brief Identifie les différentes scènes côté client.
 */
enum class SceneState : uint32_t {
	MENU = 0,
	GAME = 1,
	UNKNOWN = 2
};

/**
 * @brief Message sent by client to introduce itself to the server
 */
struct ClientHelloMessage {
	MessageType type;
	uint32_t clientId;
	char clientName[32];
};

/**
 * @brief Message sent by server to notify the start of the game
 */
struct GameStartMessage {
	MessageType type;
	uint32_t clientCount;
};

/**
 * @brief Message sent by server to assign an ID to a client
 */
struct ServerAssignIdMessage {
	MessageType type;
	uint32_t clientId;
};

/**
 * @brief Message sent by server to validate connection to a specific room
 */
struct ServerRoomAssignIdMessage {
	MessageType type;
	uint32_t roomId;
};

/**
 * @brief Message sent by client to ask the server to enter a room
 */
struct ClientRoomIdAskMessage {
	MessageType type;
	uint32_t clientId;
	uint32_t roomId;
};

/**
 * @brief Message sent by client to ask the server to send the list of available rooms
 */
struct ClientFetchRoomsMessage {
	MessageType type;
	uint32_t clientId;
};

struct ServerSendRoomsMessage {
	MessageType type;
	char jsonData[2047]; /**< JSON string (null-terminated) */
};

/**
 * @brief Message sent by client with input for the current frame
 */
struct ClientInputMessage {
	MessageType type;
	uint32_t clientId;
	uint32_t inputXBits;
	uint32_t inputYBits;
};

/**
 * @brief Message sent by server to update client state
 */
struct StateUpdateMessage {
	MessageType type;
	uint32_t clientId;
	uint32_t posXBits;
	uint32_t posYBits;
};

/**
 * @brief Message sent by server to spawn a new enemy
 */
struct EnemySpawnMessage {
	MessageType type;
	uint32_t enemyId;
	uint32_t posXBits;
	uint32_t posYBits;
	uint32_t velXBits; // Vélocité X (float bits)
	uint32_t velYBits; // Vélocité Y (float bits)
};

/**
 * @brief Message sent by server to notify that a player has died
 */
struct PlayerDeathMessage {
	MessageType type;
	uint32_t clientId; // ID du joueur qui est mort
};

/**
 * @brief Message sent by server to update an enemy's position
 */
struct EnemyUpdateMessage {
	MessageType type;
	uint32_t enemyId;
	uint32_t posXBits;
	uint32_t posYBits;
};

/**
 * @brief Message sent by server to remove an enemy
 */
struct EnemyDespawnMessage {
	MessageType type;
	uint32_t enemyId;
};

/**
 * @brief Message sent by server to spawn a static obstacle
 */
struct ObstacleSpawnMessage {
	MessageType type;
	uint32_t obstacleId;
	uint32_t posXBits;
	uint32_t posYBits;
	uint32_t widthBits;
	uint32_t heightBits;
};

/**
 * @brief Message sent by server to remove an obstacle
 */
struct ObstacleDespawnMessage {
	MessageType type; /**< Message type (ObstacleDespawn) */
	uint32_t obstacleId; /**< Obstacle ID to remove (network byte order) */
};

/**
 * @brief Message sent by client to send shoot
 */
struct ClientShootMessage {
	MessageType type; // ClientShoot
	uint32_t clientId; // ID du client qui tire (network byte order)
};

/**
 * @brief Message sent by server to create an projectile
 */
struct ProjectileSpawnMessage {
	MessageType type; // ProjectileSpawn
	uint32_t projectileId; // ID unique du projectile
	uint32_t ownerId; // ID du joueur qui a tiré
	uint32_t posXBits; // Position X (float bits)
	uint32_t posYBits; // Position Y (float bits)
	uint32_t velXBits; // Vélocité X (float bits)
	uint32_t velYBits; // Vélocité Y (float bits)
};

/**
 * @brief Message sent by server to update an projectile
 */
struct ProjectileUpdateMessage {
	MessageType type; // ProjectileUpdate
	uint32_t projectileId; // ID unique du projectile
	uint32_t posXBits; // Position X (float bits)
	uint32_t posYBits; // Position y (float bits)
};

/**
 * @brief Message sent by server to remove an projectile
 */
struct ProjectileDespawnMessage {
	MessageType type; // ProjectileDeSpawn
	uint32_t projectileId; // ID unique du projectile
};

/**
 * @brief Message sent by server to spawn an enemy projectile
 */
struct EnemyProjectileSpawnMessage {
	MessageType type; // EnemyProjectileSpawn
	uint32_t projectileId; // ID unique du projectile ennemi
	uint32_t ownerId; // ID de l'ennemi qui a tiré
	uint32_t posXBits; // Position X (float bits)
	uint32_t posYBits; // Position Y (float bits)
	uint32_t velXBits; // Vélocité X (float bits)
	uint32_t velYBits; // Vélocité Y (float bits)
};

/**
 * @brief Message sent by server to update an enemy projectile
 */
struct EnemyProjectileUpdateMessage {
	MessageType type; // EnemyProjectileUpdate
	uint32_t projectileId; // ID unique du projectile ennemi
	uint32_t posXBits; // Position X (float bits)
	uint32_t posYBits; // Position Y (float bits)
};

/**
 * @brief Message sent by server to remove an enemy projectile
 */
struct EnemyProjectileDespawnMessage {
	MessageType type; // EnemyProjectileDespawn
	uint32_t projectileId; // ID unique du projectile ennemi
};

/**
 * @brief Message sent by server with serialized entity JSON data
 */
struct EntityDataMessage {
	MessageType type;
	uint32_t dataLength; /**< Length of JSON data */
	char jsonData[2048]; /**< JSON string (null-terminated) */
};

/**
 * @brief Message envoyé par le client pour indiquer la scène courante (menu / jeu).
 */
struct SceneStateMessage {
	MessageType type;
	uint32_t clientId;
	uint32_t scene; // correspond à SceneState
};

/**
 * @brief Message sent by server to update a player's health
 */
struct PlayerHealthMessage {
	MessageType type;
	uint32_t clientId;
	int16_t currentHealth;
	int16_t maxHealth;
};

/**
 * @brief Message sent by server to update the global score
 */
struct GlobalScoreMessage {
	MessageType type;
	int32_t totalScore;
};

/**
 * @brief Message sent by server to update a player's individual score
 */
struct IndividualScoreMessage {
	MessageType type;
	uint32_t clientId;
	uint32_t score;
};
