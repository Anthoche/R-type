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
 * @brief Types of messages exchanged between client and server
 */
enum class MessageType : uint8_t {
    ClientHello,      /**< Client greeting the server */
    ServerAssignId,   /**< Server assigns an ID to the client */
    GameStart,        /**< Server notifies clients that the game is starting */
    ClientInput,      /**< Client sends input for the current frame */
    StateUpdate,      /**< Server sends updated state for a client */
    EnemySpawn,       /**< Server spawns a new enemy */
    EnemyUpdate,      /**< Server updates an enemy's position */
    EnemyDespawn,     /**< Server removes an enemy */
    ObstacleSpawn,    /**< Server spawns a new obstacle */
    ObstacleDespawn,   /**< Server removes an obstacle */
    ClientShoot,        /**< CLient send a shoot */
    ProjectileSpawn,      /**< Server create an projectile */
    ProjectileUpdate,     /**< Server update an projectile */
    ProjectileDespawn     /**< Server delete an projectile */
};

/**
 * @brief Message sent by client to introduce itself to the server
 */
struct ClientHelloMessage {
    MessageType type;          /**< Message type (ClientHello) */
    uint32_t clientId;         /**< ID of the client */
    char clientName[32];       /**< Name of the client (C-string, max 31 chars + null) */
};

/**
 * @brief Message sent by server to notify the start of the game
 */
struct GameStartMessage {
    MessageType type;          /**< Message type (GameStart) */
    uint32_t clientCount;      /**< Number of connected clients */
};

/**
 * @brief Message sent by server to assign an ID to a client
 */
struct ServerAssignIdMessage {
    MessageType type;          /**< Message type (ServerAssignId) */
    uint32_t clientId;         /**< Assigned client ID (network byte order) */
};

/**
 * @brief Message sent by client with input for the current frame
 */
struct ClientInputMessage {
    MessageType type;          /**< Message type (ClientInput) */
    uint32_t clientId;         /**< Client ID (network byte order) */
    uint32_t inputXBits;       /**< X input as float bits (network byte order) */
    uint32_t inputYBits;       /**< Y input as float bits (network byte order) */
};

/**
 * @brief Message sent by server to update client state
 */
struct StateUpdateMessage {
    MessageType type;          /**< Message type (StateUpdate) */
    uint32_t clientId;         /**< Recipient client ID (network byte order) */
    uint32_t posXBits;         /**< X position as float bits (network byte order) */
    uint32_t posYBits;         /**< Y position as float bits (network byte order) */
};

/**
 * @brief Message sent by server to spawn a new enemy
 */
struct EnemySpawnMessage {
    MessageType type;          /**< Message type (EnemySpawn) */
    uint32_t enemyId;          /**< Enemy ID (network byte order) */
    uint32_t posXBits;         /**< X position as float bits (network byte order) */
    uint32_t posYBits;         /**< Y position as float bits (network byte order) */
};

/**
 * @brief Message sent by server to update an enemy's position
 */
struct EnemyUpdateMessage {
    MessageType type;          /**< Message type (EnemyUpdate) */
    uint32_t enemyId;          /**< Enemy ID (network byte order) */
    uint32_t posXBits;         /**< X position as float bits (network byte order) */
    uint32_t posYBits;         /**< Y position as float bits (network byte order) */
};

/**
 * @brief Message sent by server to remove an enemy
 */
struct EnemyDespawnMessage {
    MessageType type;          /**< Message type (EnemyDespawn) */
    uint32_t enemyId;          /**< Enemy ID to remove (network byte order) */
};

/**
 * @brief Message sent by server to spawn a static obstacle
 */
struct ObstacleSpawnMessage {
    MessageType type;          /**< Message type (ObstacleSpawn) */
    uint32_t obstacleId;       /**< Obstacle ID (network byte order) */
    uint32_t posXBits;         /**< X position as float bits (network byte order) */
    uint32_t posYBits;         /**< Y position as float bits (network byte order) */
    uint32_t widthBits;        /**< Width as float bits (network byte order) */
    uint32_t heightBits;       /**< Height as float bits (network byte order) */
};

/**
 * @brief Message sent by server to remove an obstacle
 */
struct ObstacleDespawnMessage {
    MessageType type;          /**< Message type (ObstacleDespawn) */
    uint32_t obstacleId;       /**< Obstacle ID to remove (network byte order) */
};

/**
 * @brief Message sent by client to send shoot
 */
struct ClientShootMessage {
    MessageType type;     // ClientShoot
    uint32_t clientId;    // ID du client qui tire (network byte order)
};

/**
 * @brief Message sent by server to create an projectile
 */
struct ProjectileSpawnMessage {
    MessageType type;         // ProjectileSpawn
    uint32_t projectileId;    // ID unique du projectile
    uint32_t ownerId;         // ID du joueur qui a tiré
    uint32_t posXBits;        // Position X (float bits)
    uint32_t posYBits;        // Position Y (float bits)
    uint32_t velXBits;        // Vélocité X (float bits)
    uint32_t velYBits;        // Vélocité Y (float bits)
};

/**
 * @brief Message sent by server to update an projectile
 */
struct ProjectileUpdateMessage {
    MessageType type;       // ProjectileUpdate
    uint32_t projectileId;  // ID unique du projectile
    uint32_t posXBits;      // Position X (float bits)
    uint32_t posYBits;      // Position y (float bits)
};

/**
 * @brief Message sent by server to remove an projectile
 */
struct ProjectileDespawnMessage {
    MessageType type;       // ProjectileDeSpawn
    uint32_t projectileId;  // ID unique du projectile
};