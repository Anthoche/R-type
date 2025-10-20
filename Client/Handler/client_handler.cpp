/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** client_handler
*/

#include "../client.hpp"
#include "Logger.hpp"
#include "../../Engine/Game.hpp"
#include <asio.hpp>

void GameClient::handleMessage(MessageType type, const std::vector<uint8_t> &buffer) {
    switch (type) {
        case MessageType::ServerAssignId:
            handleServerAssignId(buffer);
            break;
        case MessageType::ServerRoomAssignId:
            handleServerRoomAssign(buffer);
            break;
        case MessageType::ServerSendRooms:
            handleServerRooms(buffer);
            break;
        case MessageType::GameStart:
            handleGameStart(buffer);
            break;
        case MessageType::StateUpdate:
            handlePlayerUpdate(buffer);
            break;
        case MessageType::ObstacleSpawn:
            handleObstacleSpawn(buffer);
            break;
        case MessageType::ObstacleDespawn:
            handleObstacleDespawn(buffer);
            break;
        case MessageType::PlayerHealth:
            handlePlayerHealth(buffer);
            break;
        case MessageType::GlobalScore:
            handleGlobalScore(buffer);
            break;
        case MessageType::IndividualScore:
            handleIndividualScore(buffer);
            break;
        case MessageType::ProjectileSpawn:
            handleProjectileSpawn(buffer);
            break;
        case MessageType::ProjectileDespawn:
            handleProjectileDespawn(buffer);
            break;
        case MessageType::ProjectileUpdate:
            handleProjectileUpdate(buffer);
            break;
        case MessageType::EnemySpawn:
            handleEnemySpawn(buffer);
            break;
        case MessageType::EnemyDespawn:
            handleEnemyDespawn(buffer);
            break;
        case MessageType::EnemyUpdate:
            handleEnemyUpdate(buffer);
            break;
        case MessageType::PlayerDeath:
            handlePlayerDeath(buffer);
            break;
        case MessageType::EnemyProjectileSpawn:
            handleEnemyProjectileSpawn(buffer);
            break;
        case MessageType::EnemyProjectileUpdate:
            handleEnemyProjectileUpdate(buffer);
            break;
        case MessageType::EnemyProjectileDespawn:
            handleEnemyProjectileDespawn(buffer);
            break;
        default:
            break;
    }
}

void GameClient::handleServerAssignId(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ServerAssignIdMessage)) return;
    const ServerAssignIdMessage *msg = reinterpret_cast<const ServerAssignIdMessage *>(buffer.data());
    clientId = ntohl(msg->clientId);
    std::cout << "[Client] Reçu clientId=" << clientId << std::endl;
    initTcpConnection();
}

void GameClient::handleServerRoomAssign(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ServerRoomAssignIdMessage)) return;
    const ServerRoomAssignIdMessage *msg = reinterpret_cast<const ServerRoomAssignIdMessage *>(buffer.data());
    roomId = static_cast<int>(msg->roomId);
    LOG_INFO(std::format("Client assigned to room {}", roomId));
}

void GameClient::handleServerRooms(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ServerSendRoomsMessage)) return;
    const ServerSendRoomsMessage *msg = reinterpret_cast<const ServerSendRoomsMessage *>(buffer.data());
    LOG_INFO("Server sent rooms");
    LOG_DEBUG(std::format("Received JSON:\n{}\n", msg->jsonData));
    //TODO: do something with msg->jsonData
}

void GameClient::handleGameStart(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(GameStartMessage)) return;
    const GameStartMessage *msg = reinterpret_cast<const GameStartMessage *>(buffer.data());
    std::cout << "Le jeu commence ! Nombre de joueurs : " << ntohl(msg->clientCount) << std::endl;
    _game.setGameStatus(GameStatus::RUNNING);
}

void GameClient::handlePlayerUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(StateUpdateMessage)) return;
    const StateUpdateMessage *msg = reinterpret_cast<const StateUpdateMessage *>(buffer.data());
    uint32_t id = ntohl(msg->clientId);
    uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
    float x, y;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::lock_guard<std::mutex> g(stateMutex);
    players[id] = {x, y};
}

void GameClient::handleObstacleSpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ObstacleSpawnMessage)) return;
    const ObstacleSpawnMessage *msg = reinterpret_cast<const ObstacleSpawnMessage *>(buffer.data());
    uint32_t id = ntohl(msg->obstacleId);
    uint32_t xb = ntohl(msg->posXBits), yb = ntohl(msg->posYBits);
    uint32_t wb = ntohl(msg->widthBits), hb = ntohl(msg->heightBits);
    float x, y, w, h;
    std::memcpy(&x, &xb, 4);
    std::memcpy(&y, &yb, 4);
    std::memcpy(&w, &wb, 4);
    std::memcpy(&h, &hb, 4);
    std::lock_guard<std::mutex> g(stateMutex);
    obstacles[id] = std::make_tuple(x, y, w, h);
}

void GameClient::handleObstacleDespawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ObstacleDespawnMessage)) return;
    const ObstacleDespawnMessage *msg = reinterpret_cast<const ObstacleDespawnMessage *>(buffer.data());
    uint32_t id = ntohl(msg->obstacleId);
    std::lock_guard<std::mutex> g(stateMutex);
    obstacles.erase(id);
}

void GameClient::handleProjectileSpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ProjectileSpawnMessage)) return;
    const ProjectileSpawnMessage *msg = reinterpret_cast<const ProjectileSpawnMessage *>(buffer.data());
    
    uint32_t projId = ntohl(msg->projectileId);
    uint32_t xb = ntohl(msg->posXBits);
    uint32_t yb = ntohl(msg->posYBits);
    uint32_t vxb = ntohl(msg->velXBits);
    uint32_t vyb = ntohl(msg->velYBits);
    
    float x, y, vx, vy;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::memcpy(&vx, &vxb, sizeof(float));
    std::memcpy(&vy, &vyb, sizeof(float));
    
    std::lock_guard<std::mutex> g(stateMutex);
    projectiles[projId] = std::make_tuple(x, y, vx, vy, clientId);
}

void GameClient::handleProjectileDespawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ProjectileDespawnMessage)) return;
    const ProjectileDespawnMessage *msg = reinterpret_cast<const ProjectileDespawnMessage *>(buffer.data());
    uint32_t projId = ntohl(msg->projectileId);
    
    std::lock_guard<std::mutex> g(stateMutex);
    projectiles.erase(projId);
}

void GameClient::handleProjectileUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ProjectileUpdateMessage)) return;
    const ProjectileUpdateMessage *msg = 
        reinterpret_cast<const ProjectileUpdateMessage *>(buffer.data());
    
    uint32_t projId = ntohl(msg->projectileId);
    uint32_t xb = ntohl(msg->posXBits);
    uint32_t yb = ntohl(msg->posYBits);
    
    float x, y;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    
    std::lock_guard<std::mutex> g(stateMutex);
    auto it = projectiles.find(projId);
    if (it != projectiles.end()) {
        std::get<0>(it->second) = x;
        std::get<1>(it->second) = y;
    }
}

void GameClient::handleEnemyProjectileSpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(EnemyProjectileSpawnMessage)) return;
    const EnemyProjectileSpawnMessage *msg = 
        reinterpret_cast<const EnemyProjectileSpawnMessage *>(buffer.data());
    
    uint32_t projId = ntohl(msg->projectileId);
    uint32_t ownerId = ntohl(msg->ownerId);
    uint32_t xb = ntohl(msg->posXBits);
    uint32_t yb = ntohl(msg->posYBits);
    uint32_t vxb = ntohl(msg->velXBits);
    uint32_t vyb = ntohl(msg->velYBits);
    
    float x, y, vx, vy;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::memcpy(&vx, &vxb, sizeof(float));
    std::memcpy(&vy, &vyb, sizeof(float));
    
    std::lock_guard<std::mutex> g(stateMutex);
    enemyProjectiles[projId] = std::make_tuple(x, y, vx, vy, ownerId);
    
    std::cout << "[Client] Enemy projectile spawned: id=" << projId 
              << " from enemy=" << ownerId 
              << " pos=(" << x << ", " << y << ")" << std::endl;
}

void GameClient::handleEnemyProjectileUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(EnemyProjectileUpdateMessage)) return;
    const EnemyProjectileUpdateMessage *msg = 
        reinterpret_cast<const EnemyProjectileUpdateMessage *>(buffer.data());
    
    uint32_t projId = ntohl(msg->projectileId);
    uint32_t xb = ntohl(msg->posXBits);
    uint32_t yb = ntohl(msg->posYBits);
    
    float x, y;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    
    std::lock_guard<std::mutex> g(stateMutex);
    auto it = enemyProjectiles.find(projId);
    if (it != enemyProjectiles.end()) {
        std::get<0>(it->second) = x;
        std::get<1>(it->second) = y;
    }
}

void GameClient::handleEnemyProjectileDespawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(EnemyProjectileDespawnMessage)) return;
    const EnemyProjectileDespawnMessage *msg = 
        reinterpret_cast<const EnemyProjectileDespawnMessage *>(buffer.data());
    uint32_t projId = ntohl(msg->projectileId);
    
    std::lock_guard<std::mutex> g(stateMutex);
    enemyProjectiles.erase(projId);
    
    std::cout << "[Client] Enemy projectile despawned: id=" << projId << std::endl;
}

void GameClient::handleEnemySpawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(EnemySpawnMessage)) return;
    const EnemySpawnMessage *msg = reinterpret_cast<const EnemySpawnMessage *>(buffer.data());
    
    uint32_t enemyId = ntohl(msg->enemyId);
    uint32_t xb = ntohl(msg->posXBits);
    uint32_t yb = ntohl(msg->posYBits);
    uint32_t vxb = ntohl(msg->velXBits);
    uint32_t vyb = ntohl(msg->velYBits);
    
    float x, y, vx, vy;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    std::memcpy(&vx, &vxb, sizeof(float));
    std::memcpy(&vy, &vyb, sizeof(float));
    
    std::lock_guard<std::mutex> g(stateMutex);
    enemies[enemyId] = std::make_tuple(x, y, vx, vy);
    
    std::cout << "[Client] Enemy spawned: id=" << enemyId 
              << " pos=(" << x << ", " << y << ")" << std::endl;
}

void GameClient::handleEnemyDespawn(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(EnemyDespawnMessage)) return;
    const EnemyDespawnMessage *msg = reinterpret_cast<const EnemyDespawnMessage *>(buffer.data());
    uint32_t enemyId = ntohl(msg->enemyId);
    
    std::lock_guard<std::mutex> g(stateMutex);
    enemies.erase(enemyId);
    
    std::cout << "[Client] Enemy despawned: id=" << enemyId << std::endl;
}

void GameClient::handleEnemyUpdate(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(EnemyUpdateMessage)) return;
    const EnemyUpdateMessage *msg = 
        reinterpret_cast<const EnemyUpdateMessage *>(buffer.data());
    
    uint32_t enemyId = ntohl(msg->enemyId);
    uint32_t xb = ntohl(msg->posXBits);
    uint32_t yb = ntohl(msg->posYBits);
    
    float x, y;
    std::memcpy(&x, &xb, sizeof(float));
    std::memcpy(&y, &yb, sizeof(float));
    
    std::lock_guard<std::mutex> g(stateMutex);
    auto it = enemies.find(enemyId);
    if (it != enemies.end()) {
        std::get<0>(it->second) = x;
        std::get<1>(it->second) = y;
    }
}

void GameClient::handlePlayerDeath(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(PlayerDeathMessage)) return;
    const PlayerDeathMessage *msg = reinterpret_cast<const PlayerDeathMessage *>(buffer.data());
    uint32_t deadPlayerId = ntohl(msg->clientId);
    
    std::lock_guard<std::mutex> g(stateMutex);
    players.erase(deadPlayerId);
    
    std::cout << "[Client] Player " << deadPlayerId << " died!" << std::endl;
    
    if (deadPlayerId == clientId) {
        std::cout << "[Client] YOU DIED!" << std::endl;
    }
}

void GameClient::handlePlayerHealth(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(PlayerHealthMessage)) return;
    const PlayerHealthMessage *msg = reinterpret_cast<const PlayerHealthMessage *>(buffer.data());
    
    uint32_t playerId = ntohl(msg->clientId);
    int16_t currentHealth = ntohs(msg->currentHealth);
    int16_t maxHealth = ntohs(msg->maxHealth);
    
    std::lock_guard<std::mutex> g(stateMutex);
    
    playerHealth[playerId] = {currentHealth, maxHealth};
}

void GameClient::handleGlobalScore(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(GlobalScoreMessage)) return;
    const GlobalScoreMessage *msg = reinterpret_cast<const GlobalScoreMessage *>(buffer.data());
    
    int32_t globalScore = ntohl(msg->totalScore);
    
    std::lock_guard<std::mutex> g(stateMutex);
    this->globalScore = globalScore;
}

void GameClient::handleIndividualScore(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(IndividualScoreMessage)) return;
    const IndividualScoreMessage *msg = reinterpret_cast<const IndividualScoreMessage *>(buffer.data());
    
    uint32_t playerId = ntohl(msg->clientId);
    uint32_t score = ntohl(msg->score);
    
    std::lock_guard<std::mutex> g(stateMutex);
    playerIndividualScores[playerId] = score;
}