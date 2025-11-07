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
 * @enum MessageType
 * @brief Defines all network message types exchanged between client and server.
 *
 * This enumeration identifies the purpose of each network packet in the R-Type protocol.
 * Messages are categorized into connection management, room management, gameplay updates,
 * entity synchronization, player customization, and chat communication.
 */
enum class MessageType : uint8_t {
    ClientHello,              ///< Client sends initial greeting to server.
    ClientRoomIdAsk,          ///< Client requests to join a specific room.
    ClientRoomCreate,         ///< Client requests to create a new room.
    ServerRoomAssignId,       ///< Server confirms room assignment.
    ClientConfirmStart,       ///< Client confirms ready to start game.
    ClientFetchRooms,         ///< Client requests list of available rooms.
    ServerSendRooms,          ///< Server sends room list to client.
    ServerSetClientConfirmed, ///< Server marks a client as ready.
    ServerSetRoomReady,       ///< Server notifies that room is ready to start.
    ServerAssignId,           ///< Server assigns a unique client ID.
    ClientLeaveRoom,          ///< Client notifies server of leaving room.
    GameStart,                ///< Server signals game has started.
    ClientInput,              ///< Client sends player input for current frame.
    StateUpdate,              ///< Server sends updated player state.
    EnemySpawn,               ///< Server spawns a new enemy entity.
    EnemyUpdate,              ///< Server updates enemy position/state.
    EnemyDespawn,             ///< Server removes an enemy entity.
    ObstacleSpawn,            ///< Server spawns a new obstacle entity.
    ObstacleUpdate,           ///< Server updates obstacle position/state.
    ObstacleDespawn,          ///< Server removes an obstacle entity.
    ClientShoot,              ///< Client initiates shooting action.
    ProjectileSpawn,          ///< Server spawns a player projectile.
    ProjectileUpdate,         ///< Server updates projectile position.
    ProjectileDespawn,        ///< Server removes a projectile.
    EnemyProjectileSpawn,     ///< Server spawns an enemy projectile.
    EnemyProjectileUpdate,    ///< Server updates enemy projectile position.
    EnemyProjectileDespawn,   ///< Server removes an enemy projectile.
    BossDeath,                ///< Server notifies of boss defeat.
    ElementSpawn,             ///< Server spawns a pattern element entity.
    ElementUpdate,            ///< Server updates pattern element position/state.
    ElementDespawn,           ///< Server removes a pattern element entity.
    EndlessMode,              ///< Server/client synchronizes endless mode setting.
    EntityData,               ///< Server sends full ECS entity synchronization.
    PlayerWeaponUpdate,       ///< Client/server synchronizes weapon selection.
    PlayerSkinUpdate,         ///< Client/server synchronizes skin selection.
    SceneState,               ///< Client notifies server of current scene (menu/game).
    PlayerDeath,              ///< Server notifies of player death.
    PlayerHealth,             ///< Server updates player health values.
    InitialHealth,            ///< Client sends initial health to server.
    GlobalScore,              ///< Server updates team global score.
    IndividualScore,          ///< Server updates individual player score.
    ChatMessage,              ///< Chat message relayed through server.
};

/**
 * @enum InputCode
 * @brief Identifies raw player input events sent from client to server.
 *
 * Represents directional movement inputs for player control.
 */
enum class InputCode : uint8_t {
    Up = 0,
    Down = 1,
    Left = 2,
    Right = 3,
    J = 4,
    K = 5,
    Unknown = 255
};

/**
 * @brief Converts an InputCode to its string representation.
 *
 * @param code The input code to convert.
 * @return Human-readable string name of the input.
 */
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
        case InputCode::J:
            return "J";
        case InputCode::K:
            return "K";
        default:
            return "Unknown";
    }
}

/**
 * @enum SceneState
 * @brief Identifies the different scenes on the client side.
 *
 * Used to synchronize which screen/state the client is currently displaying.
 */
enum class SceneState : uint32_t {
    MENU = 0,    ///< Main menu or lobby screen.
    GAME = 1,    ///< Active gameplay.
    UNKNOWN = 2  ///< Unrecognized or transitional state.
};

/**
 * @struct Position3D
 * @brief Represents a 3D position with float coordinates transmitted as bit patterns.
 *
 * Float values are transmitted as their raw bit representation to ensure
 * exact cross-platform serialization without precision loss.
 */
struct Position3D {
    uint32_t xBits; ///< X coordinate (float encoded as uint32_t bits).
    uint32_t yBits; ///< Y coordinate (float encoded as uint32_t bits).
    uint32_t zBits; ///< Z coordinate (float encoded as uint32_t bits).
};

/**
 * @struct Velocity3D
 * @brief Represents a 3D velocity vector with float components transmitted as bit patterns.
 */
struct Velocity3D {
    uint32_t vxBits; ///< Velocity X component (float encoded as uint32_t bits).
    uint32_t vyBits; ///< Velocity Y component (float encoded as uint32_t bits).
    uint32_t vzBits; ///< Velocity Z component (float encoded as uint32_t bits).
};

/**
 * @struct Size3D
 * @brief Represents 3D dimensions with float values transmitted as bit patterns.
 */
struct Size3D {
    uint32_t widthBits;  ///< Width dimension (float encoded as uint32_t bits).
    uint32_t heightBits; ///< Height dimension (float encoded as uint32_t bits).
    uint32_t depthBits;  ///< Depth dimension (float encoded as uint32_t bits).
};

/**
 * @struct ChatMessagePacket
 * @brief Chat message exchanged between clients via server relay.
 *
 * Strings are null-terminated UTF-8 encoded. Fixed-size buffers ensure
 * trivial serialization across different platforms and compilers.
 */
struct ChatMessagePacket {
    MessageType type;      ///< Always MessageType::ChatMessage.
    uint32_t senderId;     ///< Client ID of the message sender (network byte order).
    char senderName[32];   ///< Display name of the sender (null-terminated).
    char message[256];     ///< Chat message content (null-terminated).
};

/**
 * @struct ClientHelloMessage
 * @brief Initial message sent by client to introduce itself to the server.
 */
struct ClientHelloMessage {
    MessageType type;      ///< Always MessageType::ClientHello.
    uint32_t clientId;     ///< Client ID (initially 0, assigned by server).
    char clientName[32];   ///< Display name of the client (null-terminated).
};

/**
 * @struct PlayerSkinMessage
 * @brief Message to synchronize player skin selection between client and server.
 */
struct PlayerSkinMessage {
    MessageType type;         ///< Always MessageType::PlayerSkinUpdate.
    uint32_t clientId;        ///< Client ID of the player changing skin.
    char skinFilename[64];    ///< Filename of the selected skin (null-terminated).
};

/**
 * @struct PlayerWeaponMessage
 * @brief Message to synchronize player weapon selection between client and server.
 */
struct PlayerWeaponMessage {
    MessageType type;      ///< Always MessageType::PlayerWeaponUpdate.
    uint32_t clientId;     ///< Client ID of the player changing weapon.
    char weaponId[32];     ///< Identifier of the selected weapon (null-terminated).
};

/**
 * @struct GameStartMessage
 * @brief Message sent by server to notify all clients that the game is starting.
 */
struct GameStartMessage {
    MessageType type;      ///< Always MessageType::GameStart.
    uint32_t clientCount;  ///< Number of players in the game.
    uint32_t roomId;       ///< Room ID of the starting game.
};

/**
 * @struct RoomReadyMessage
 * @brief Message sent by server to notify that the room is ready to start.
 */
struct RoomReadyMessage {
    MessageType type;  ///< Always MessageType::ServerSetRoomReady.
    uint32_t roomId;   ///< Room ID that is ready.
};

/**
 * @struct ClientConfirmStartMessage
 * @brief Message sent by client to confirm readiness to start the game.
 */
struct ClientConfirmStartMessage {
    MessageType type;  ///< Always MessageType::ClientConfirmStart.
    uint32_t clientId; ///< Client ID confirming readiness.
    uint32_t roomId;   ///< Room ID the client is ready in.
};

/**
 * @struct ServerAssignIdMessage
 * @brief Message sent by server to assign a unique ID to a newly connected client.
 */
struct ServerAssignIdMessage {
    MessageType type;  ///< Always MessageType::ServerAssignId.
    uint32_t clientId; ///< Assigned client ID.
};

/**
 * @struct ServerRoomAssignIdMessage
 * @brief Message sent by server to confirm room assignment to a client.
 */
struct ServerRoomAssignIdMessage {
    MessageType type; ///< Always MessageType::ServerRoomAssignId.
    uint32_t roomId;  ///< Assigned room ID.
};

/**
 * @struct ClientRoomIdAskMessage
 * @brief Message sent by client to request joining a specific room.
 */
struct ClientRoomIdAskMessage {
    MessageType type;  ///< Always MessageType::ClientRoomIdAsk.
    uint32_t clientId; ///< Client ID requesting room access.
    uint32_t roomId;   ///< Desired room ID to join.
};

/**
 * @struct ClientLeaveRoomMessage
 * @brief Message sent by client to notify server of leaving the current room.
 */
struct ClientLeaveRoomMessage {
    MessageType type;  ///< Always MessageType::ClientLeaveRoom.
    uint32_t clientId; ///< Client ID leaving the room.
    uint32_t roomId;   ///< Room ID being left.
};

/**
 * @struct ClientRoomCreateMessage
 * @brief Message sent by client to request creation of a new room.
 */
struct ClientRoomCreateMessage {
    MessageType type;  ///< Always MessageType::ClientRoomCreate.
    uint32_t clientId; ///< Client ID requesting room creation.
    uint16_t minPlayers; ///< Desired minimum player count.
    uint16_t maxPlayers; ///< Desired maximum player count.
};

/**
 * @struct ClientFetchRoomsMessage
 * @brief Message sent by client to request the list of available rooms.
 */
struct ClientFetchRoomsMessage {
    MessageType type;  ///< Always MessageType::ClientFetchRooms.
    uint32_t clientId; ///< Client ID requesting room list.
};

/**
 * @struct ServerSendRoomsMessage
 * @brief Message sent by server containing the list of available rooms as JSON.
 */
struct ServerSendRoomsMessage {
    MessageType type;    ///< Always MessageType::ServerSendRooms.
    char jsonData[2047]; ///< JSON-formatted room list (null-terminated).
};

/**
 * @struct ClientInputMessage
 * @brief Message sent by client containing player input for the current frame.
 */
struct ClientInputMessage {
    MessageType type;    ///< Always MessageType::ClientInput.
    uint32_t clientId;   ///< Client ID sending input.
    uint8_t inputCode;   ///< Input type (see InputCode enum).
    uint8_t isPressed;   ///< 1 if pressed, 0 if released.
    uint16_t padding{0}; ///< Padding for alignment.
};

/**
 * @struct StateUpdateMessage
 * @brief Message sent by server to update a client's 3D position.
 */
struct StateUpdateMessage {
    MessageType type;  ///< Always MessageType::StateUpdate.
    uint32_t clientId; ///< Client ID being updated.
    Position3D pos;    ///< New 3D position.
};

/**
 * @struct EnemySpawnMessage
 * @brief Message sent by server to spawn a new enemy with initial position and velocity.
 */
struct EnemySpawnMessage {
    MessageType type;  ///< Always MessageType::EnemySpawn.
    uint32_t enemyId;  ///< Unique enemy identifier.
    Position3D pos;    ///< Initial 3D position.
    Velocity3D vel;    ///< Initial 3D velocity.
    float width;       ///< Enemy width.
    float height;      ///< Enemy height.
};

/**
 * @struct ElementSpawnMessage
 * @brief Message sent by server to spawn a new pattern element entity.
 *
 * Pattern elements are entities that move in coordinated formations or follow
 * specific movement patterns as part of enemy attack waves or decorative elements.
 */
struct ElementSpawnMessage {
    MessageType type;   ///< Always MessageType::ElementSpawn.
    uint32_t elementId; ///< Unique pattern element identifier.
    Position3D pos;     ///< Initial 3D position.
    Velocity3D vel;     ///< Initial 3D velocity.
    float width;        ///< Element width.
    float height;       ///< Element height.
};

/**
 * @struct ElementDespawnMessage
 * @brief Message sent by server to remove a pattern element from the game.
 */
struct ElementDespawnMessage {
    MessageType type;   ///< Always MessageType::ElementDespawn.
    uint32_t elementId; ///< Element identifier to remove.
};

/**
 * @struct ElementUpdateMessage
 * @brief Message sent by server to update a pattern element's position and velocity.
 */
struct ElementUpdateMessage {
    MessageType type;   ///< Always MessageType::ElementUpdate.
    uint32_t elementId; ///< Element identifier.
    Position3D pos;     ///< Updated 3D position.
    uint32_t velXBits;  ///< Velocity X component (float encoded as bits).
    uint32_t velYBits;  ///< Velocity Y component (float encoded as bits).
};

/**
 * @struct EnemyUpdateMessage
 * @brief Message sent by server to update an enemy's position and velocity.
 */
struct EnemyUpdateMessage {
    MessageType type;     ///< Always MessageType::EnemyUpdate.
    uint32_t enemyId;     ///< Enemy identifier.
    Position3D pos;       ///< Updated 3D position.
    uint32_t velXBits;    ///< Velocity X component (float encoded as bits).
    uint32_t velYBits;    ///< Velocity Y component (float encoded as bits).
};

/**
 * @struct EnemyDespawnMessage
 * @brief Message sent by server to remove an enemy from the game.
 */
struct EnemyDespawnMessage {
    MessageType type;  ///< Always MessageType::EnemyDespawn.
    uint32_t enemyId;  ///< Enemy identifier to remove.
};

/**
 * @struct BossDeathMessage
 * @brief Message sent by server to notify of a boss defeat.
 */
struct BossDeathMessage {
    MessageType type;       ///< Always MessageType::BossDeath.
    uint32_t bossId;        ///< Boss identifier that was defeated.
    bool _lastBoss = false; ///< Whether this was the final boss.
};

/**
 * @struct ObstacleSpawnMessage
 * @brief Message sent by server to spawn a new obstacle with position, size, and velocity.
 */
struct ObstacleSpawnMessage {
    MessageType type;      ///< Always MessageType::ObstacleSpawn.
    uint32_t obstacleId;   ///< Unique obstacle identifier.
    Position3D pos;        ///< Initial 3D position.
    Size3D size;           ///< 3D dimensions.
    Velocity3D vel;        ///< 3D velocity (for moving obstacles).
};

/**
 * @struct ObstacleUpdateMessage
 * @brief Message sent by server to update an obstacle's position and velocity.
 */
struct ObstacleUpdateMessage {
    MessageType type;    ///< Always MessageType::ObstacleUpdate.
    uint32_t obstacleId; ///< Obstacle identifier.
    Position3D pos;      ///< Updated 3D position.
    Velocity3D vel;      ///< Updated 3D velocity.
};

/**
 * @struct ObstacleDespawnMessage
 * @brief Message sent by server to remove an obstacle from the game.
 */
struct ObstacleDespawnMessage {
    MessageType type;    ///< Always MessageType::ObstacleDespawn.
    uint32_t obstacleId; ///< Obstacle identifier to remove.
};

/**
 * @struct ClientShootMessage
 * @brief Message sent by client to initiate a shooting action at a specific position.
 */
struct ClientShootMessage {
    MessageType type;    ///< Always MessageType::ClientShoot.
    uint32_t clientId;   ///< Client ID performing the shoot.
    Position3D shootPos; ///< 3D position where the projectile originates.
};

/**
 * @struct ProjectileSpawnMessage
 * @brief Message sent by server to create a player projectile.
 */
struct ProjectileSpawnMessage {
    MessageType type;       ///< Always MessageType::ProjectileSpawn.
    uint32_t projectileId;  ///< Unique projectile identifier.
    uint32_t ownerId;       ///< Client ID of the projectile owner.
    Position3D pos;         ///< Initial 3D position.
    Velocity3D vel;         ///< Initial 3D velocity.
};

/**
 * @struct ProjectileUpdateMessage
 * @brief Message sent by server to update a projectile's position.
 */
struct ProjectileUpdateMessage {
    MessageType type;      ///< Always MessageType::ProjectileUpdate.
    uint32_t projectileId; ///< Projectile identifier.
    Position3D pos;        ///< Updated 3D position.
};

/**
 * @struct ProjectileDespawnMessage
 * @brief Message sent by server to remove a projectile from the game.
 */
struct ProjectileDespawnMessage {
    MessageType type;      ///< Always MessageType::ProjectileDespawn.
    uint32_t projectileId; ///< Projectile identifier to remove.
};

/**
 * @struct EnemyProjectileSpawnMessage
 * @brief Message sent by server to spawn an enemy projectile.
 */
struct EnemyProjectileSpawnMessage {
    MessageType type;      ///< Always MessageType::EnemyProjectileSpawn.
    uint32_t projectileId; ///< Unique projectile identifier.
    uint32_t ownerId;      ///< Enemy ID that fired the projectile.
    Position3D pos;        ///< Initial 3D position.
    Velocity3D vel;        ///< Initial 3D velocity.
};

/**
 * @struct EnemyProjectileUpdateMessage
 * @brief Message sent by server to update an enemy projectile's position.
 */
struct EnemyProjectileUpdateMessage {
    MessageType type;      ///< Always MessageType::EnemyProjectileUpdate.
    uint32_t projectileId; ///< Projectile identifier.
    Position3D pos;        ///< Updated 3D position.
};

/**
 * @struct EnemyProjectileDespawnMessage
 * @brief Message sent by server to remove an enemy projectile.
 */
struct EnemyProjectileDespawnMessage {
    MessageType type;      ///< Always MessageType::EnemyProjectileDespawn.
    uint32_t projectileId; ///< Projectile identifier to remove.
};

/**
 * @struct EntityDataMessage
 * @brief Message sent by server containing serialized ECS entity data as JSON.
 */
struct EntityDataMessage {
    MessageType type;    ///< Always MessageType::EntityData.
    uint32_t dataLength; ///< Length of the JSON data.
    char jsonData[2048]; ///< JSON-formatted entity data (null-terminated).
};

/**
 * @struct SceneStateMessage
 * @brief Message sent by client to indicate the current scene (menu or game).
 */
struct SceneStateMessage {
    MessageType type;  ///< Always MessageType::SceneState.
    uint32_t clientId; ///< Client ID reporting scene state.
    uint32_t scene;    ///< Scene identifier (see SceneState enum).
};

/**
 * @struct PlayerDeathMessage
 * @brief Message sent by server to notify that a player has died.
 */
struct PlayerDeathMessage {
    MessageType type;  ///< Always MessageType::PlayerDeath.
    uint32_t clientId; ///< Client ID of the player who died.
};

/**
 * @struct PlayerHealthMessage
 * @brief Message sent by server to update a player's health values.
 */
struct PlayerHealthMessage {
    MessageType type;       ///< Always MessageType::PlayerHealth.
    uint32_t clientId;      ///< Client ID of the player.
    int16_t currentHealth;  ///< Current health points.
    int16_t maxHealth;      ///< Maximum health points.
};

/**
 * @struct InitialHealthMessage
 * @brief Message sent by client to inform server of initial health value.
 */
struct InitialHealthMessage {
    MessageType type;       ///< Always MessageType::InitialHealth.
    uint32_t clientId;      ///< Client ID.
    size_t initialHealth;   ///< Starting health value.
};

/**
 * @struct GlobalScoreMessage
 * @brief Message sent by server to update the team's global score.
 */
struct GlobalScoreMessage {
    MessageType type;   ///< Always MessageType::GlobalScore.
    int32_t totalScore; ///< Current global team score.
};

/**
 * @struct IndividualScoreMessage
 * @brief Message sent by server to update a player's individual score.
 */
struct IndividualScoreMessage {
    MessageType type;  ///< Always MessageType::IndividualScore.
    uint32_t clientId; ///< Client ID of the player.
    uint32_t score;    ///< Player's individual score.
};

/**
 * @struct EndlessModeMessage
 * @brief Message to synchronize endless mode setting between client and server.
 */
struct EndlessModeMessage {
    MessageType type;   ///< Always MessageType::EndlessMode.
    uint32_t clientId;  ///< Client ID.
    uint8_t isEndless;  ///< 1 if endless mode enabled, 0 otherwise.
};
