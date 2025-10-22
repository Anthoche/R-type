#pragma once
#include <cstdint>
#include <string>

/**
 * @brief Enum for all network message types exchanged between client and server.
 */
enum class MessageType : uint8_t {
    ClientHello,              /**< Client greeting the server */
    ServerAssignId,           /**< Server assigns an ID to the client */
    GameStart,                /**< Server notifies clients that the game is starting */
    ClientInput,              /**< Client sends input for the current frame */
    StateUpdate,              /**< Server sends updated state for a client */
    EnemySpawn,               /**< Server spawns a new enemy */
    EnemyUpdate,              /**< Server updates an enemy's position */
    EnemyDespawn,             /**< Server removes an enemy */
    ObstacleSpawn,            /**< Server spawns a new obstacle */
    ObstacleDespawn,          /**< Server removes an obstacle */
    ClientShoot,              /**< Client sends a shoot event */
    ProjectileSpawn,          /**< Server creates a projectile */
    ProjectileUpdate,         /**< Server updates a projectile */
    ProjectileDespawn,        /**< Server removes a projectile */
    EnemyProjectileSpawn,     /**< Server spawns an enemy projectile */
    EnemyProjectileUpdate,    /**< Server updates an enemy projectile */
    EnemyProjectileDespawn,   /**< Server removes an enemy projectile */
    EntityData,               /**< Server → Clients: entity ECS synchronization */
    SceneState,               /**< Client → Server: indicates current scene */
    PlayerDeath,              /**< Server notifies clients that a player has died */
    PlayerHealth,             /**< Server updates a player's health */
    InitialHealth,            /**< Client sends its initial health to the server */
    GlobalScore,              /**< Server updates the global score */
    IndividualScore           /**< Server updates a player's individual score */
};

/**
 * @brief Identifier for raw player input events sent from client to server.
 */
enum class InputCode : uint8_t {
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
    Unknown = 255
};

inline const char* inputCodeToString(InputCode code) {
    switch (code) {
        case InputCode::Up:
            return "Up";
        case InputCode::Down:
            return "Down";
        case InputCode::Left:
            return "Left";
        case InputCode::Right:
            return "Right";
        default:
            return "Unknown";
    }
}

/**
 * @brief Identifies the different scenes on the client side.
 */
enum class SceneState : uint32_t {
    MENU = 0,
    GAME = 1,
    UNKNOWN = 2
};

/**
 * @brief Helper struct to represent a 3D position (x, y, z).
 */
struct Position3D {
    uint32_t xBits;  ///< X position (float bits)
    uint32_t yBits;  ///< Y position (float bits)
    uint32_t zBits;  ///< Z position (float bits)
};

/**
 * @brief Helper struct to represent a 3D velocity (vx, vy, vz).
 */
struct Velocity3D {
    uint32_t vxBits;  ///< Velocity X (float bits)
    uint32_t vyBits;  ///< Velocity Y (float bits)
    uint32_t vzBits;  ///< Velocity Z (float bits)
};

/**
 * @brief Helper struct to represent a 3D size (width, height, depth).
 */
struct Size3D {
    uint32_t widthBits;   ///< Width (float bits)
    uint32_t heightBits;  ///< Height (float bits)
    uint32_t depthBits;   ///< Depth (float bits)
};

/**
 * @brief Message sent by client to introduce itself to the server.
 */
struct ClientHelloMessage {
    MessageType type;
    uint32_t clientId;
    char clientName[32];
};

/**
 * @brief Message sent by server to notify the start of the game.
 */
struct GameStartMessage {
    MessageType type;
    uint32_t clientCount;
};

/**
 * @brief Message sent by server to assign an ID to a client.
 */
struct ServerAssignIdMessage {
    MessageType type;
    uint32_t clientId;
};

/**
 * @brief Message sent by client with input for the current frame.
 */
struct ClientInputMessage {
    MessageType type;
    uint32_t clientId;
    uint8_t inputCode;
    uint8_t isPressed;
    uint16_t padding{0};
};

/**
 * @brief Message sent by server to update client state (3D position).
 */
struct StateUpdateMessage {
    MessageType type;
    uint32_t clientId;
    Position3D pos;  ///< 3D position
};

/**
 * @brief Message sent by server to spawn a new enemy (3D position and velocity).
 */
struct EnemySpawnMessage {
    MessageType type;
    uint32_t enemyId;
    Position3D pos;      ///< 3D position
    Velocity3D vel;      ///< 3D velocity
    float width;
    float height;
};

/**
 * @brief Message sent by server to update an enemy's position (3D).
 */
struct EnemyUpdateMessage {
    MessageType type;
    uint32_t enemyId;
    Position3D pos;      ///< 3D position
    uint32_t velXBits;        // Vélocité X (float bits)
    uint32_t velYBits;        // Vélocité Y (float bits)
};

/**
 * @brief Message sent by server to remove an enemy.
 */
struct EnemyDespawnMessage {
    MessageType type;
    uint32_t enemyId;
};

/**
 * @brief Message sent by server to spawn a static obstacle (3D position and size).
 */
struct ObstacleSpawnMessage {
    MessageType type;
    uint32_t obstacleId;
    Position3D pos;      ///< 3D position
    Size3D size;         ///< 3D size
};

/**
 * @brief Message sent by server to remove an obstacle.
 */
struct ObstacleDespawnMessage {
    MessageType type;
    uint32_t obstacleId;
};

/**
 * @brief Message sent by client to send a shoot event (3D position).
 */
struct ClientShootMessage {
    MessageType type;
    uint32_t clientId;
    Position3D shootPos;  ///< 3D position where the shoot originates
};

/**
 * @brief Message sent by server to create a projectile (3D position and velocity).
 */
struct ProjectileSpawnMessage {
    MessageType type;
    uint32_t projectileId;
    uint32_t ownerId;
    Position3D pos;      ///< 3D position
    Velocity3D vel;      ///< 3D velocity
};

/**
 * @brief Message sent by server to update a projectile (3D position).
 */
struct ProjectileUpdateMessage {
    MessageType type;
    uint32_t projectileId;
    Position3D pos;      ///< 3D position
};

/**
 * @brief Message sent by server to remove a projectile.
 */
struct ProjectileDespawnMessage {
    MessageType type;
    uint32_t projectileId;
};

/**
 * @brief Message sent by server to spawn an enemy projectile (3D position and velocity).
 */
struct EnemyProjectileSpawnMessage {
    MessageType type;
    uint32_t projectileId;
    uint32_t ownerId;
    Position3D pos;      ///< 3D position
    Velocity3D vel;      ///< 3D velocity
};

/**
 * @brief Message sent by server to update an enemy projectile (3D position).
 */
struct EnemyProjectileUpdateMessage {
    MessageType type;
    uint32_t projectileId;
    Position3D pos;      ///< 3D position
};

/**
 * @brief Message sent by server to remove an enemy projectile.
 */
struct EnemyProjectileDespawnMessage {
    MessageType type;
    uint32_t projectileId;
};

/**
 * @brief Message sent by server with serialized entity JSON data.
 */
struct EntityDataMessage {
    MessageType type;
    uint32_t dataLength;     ///< Length of JSON data
    char jsonData[2048];    ///< JSON string (null-terminated)
};

/**
 * @brief Message sent by client to indicate the current scene (menu/game).
 */
struct SceneStateMessage {
    MessageType type;
    uint32_t clientId;
    uint32_t scene;  ///< Corresponds to SceneState
};

/**
 * @brief Message sent by server to notify that a player has died.
 */
struct PlayerDeathMessage {
    MessageType type;
    uint32_t clientId;
};

/**
 * @brief Message sent by server to update a player's health.
 */
struct PlayerHealthMessage {
    MessageType type;
    uint32_t clientId;
    int16_t currentHealth;
    int16_t maxHealth;
};

struct InitialHealthMessage {
    MessageType type;
    uint32_t clientId;
    int16_t initialHealth;
};

/**
 * @brief Message sent by server to update the global score.
 */
struct GlobalScoreMessage {
    MessageType type;
    int32_t totalScore;
};

/**
 * @brief Message sent by server to update a player's individual score.
 */
struct IndividualScoreMessage {
    MessageType type;
    uint32_t clientId;
    uint32_t score;
};
