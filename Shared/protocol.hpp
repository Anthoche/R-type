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
    ClientHello,        /**< Client → Server: handshake initial */
    ServerAssignId,     /**< Server → Client: attribution d’un ID unique */
    GameStart,          /**< Server → Clients: lancement de la partie */
    ClientInput,        /**< Client → Server: mouvement / action */
    StateUpdate,        /**< Server → Clients: mise à jour de position */
    EnemySpawn,         /**< Server → Clients: apparition d’un ennemi */
    EnemyUpdate,        /**< Server → Clients: mise à jour ennemi */
    EnemyDespawn,       /**< Server → Clients: suppression ennemi */
    ObstacleSpawn,      /**< Server → Clients: apparition obstacle */
    ObstacleDespawn,    /**< Server → Clients: suppression obstacle */
    EntityData,         /**< Server → Clients: synchronisation entité ECS */
    SceneState,         /**< Client → Server: indique la scène courante */
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
    MessageType type;
    uint32_t obstacleId;
};

/**
 * @brief Message sent by server with serialized entity JSON data
 */
struct EntityDataMessage {
    MessageType type;
    uint32_t dataLength;     /**< Length of JSON data */
    char jsonData[2048];     /**< JSON string (null-terminated) */
};

/**
 * @brief Message envoyé par le client pour indiquer la scène courante (menu / jeu).
 */
struct SceneStateMessage {
    MessageType type;
    uint32_t clientId;
    uint32_t scene; // correspond à SceneState
};
