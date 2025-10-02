/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** client_handlers - Message handling logic
*/

#include "../client.hpp"
#include "../../Engine/Game.hpp"
#include "../../Engine/Utils/Include/entity_parser.hpp"
#include "../../Engine/Scene/Include/GameScene.hpp"
#include <nlohmann/json.hpp>

void GameClient::handleMessage(MessageType type, const std::vector<uint8_t> &buffer) {
    switch (type) {
        case MessageType::ServerAssignId:
            handleServerAssignId(buffer);
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
        case MessageType::EntityData: {
            if (buffer.size() < sizeof(EntityDataMessage)) return;
            const EntityDataMessage *msg = reinterpret_cast<const EntityDataMessage*>(buffer.data());
            std::string jsonStr(msg->jsonData, msg->dataLength);
            try {
                nlohmann::json j = nlohmann::json::parse(jsonStr);
                if (!j.contains("type")) {
                    throw std::runtime_error("Entity JSON sans champ 'type'");
                }
                ecs::entity_t e = handleEntityCreation(j);
                // Vérifier si l'entité est valide (différente de 0)
                if (e.value() != 0) {
                    std::cout << "[Client] Entity reçue et reconstruite: " << j.dump() << std::endl;
                }
            } catch (const std::exception &e) {
                std::cerr << "[Client] Erreur JSON: " << e.what() << std::endl;
            }
            break;
        }
        default:
            break;
    }
}

void GameClient::handleServerAssignId(const std::vector<uint8_t> &buffer) {
    if (buffer.size() < sizeof(ServerAssignIdMessage)) return;
    const ServerAssignIdMessage *msg = reinterpret_cast<const ServerAssignIdMessage *>(buffer.data());
    clientId = ntohl(msg->clientId);
    std::cout << "[Client] Reçu clientId=" << clientId << std::endl;
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

ecs::entity_t GameClient::handleEntityCreation(const nlohmann::json& j)
{
    auto gameScene = std::dynamic_pointer_cast<game::scene::GameScene>(
        _game.getSceneHandler().getScene("game")
    );
    
    if (!gameScene) {
        std::cerr << "[Client] GameScene introuvable" << std::endl;
        return ecs::entity_t{0};
    }
    
    ecs::registry& reg = gameScene->get_registry();
    
    auto typeVal = static_cast<component::entity_type>(j["type"].get<int>());
    switch (typeVal) {
        case component::entity_type::PLAYER:
            return game::parsing::parse_player(reg, j);
        case component::entity_type::ENEMY:
            return game::parsing::parse_enemy(reg, j);
        case component::entity_type::OBSTACLE:
            return game::parsing::parse_obstacle(reg, j);
        case component::entity_type::BACKGROUND:
            return game::parsing::parse_background(reg, j);
        case component::entity_type::SOUND:
            return game::parsing::parse_sound(reg, j);
        case component::entity_type::TEXT:
            return game::parsing::parse_text(reg, j);
        default:
            std::cerr << "[Client] Type d'entité non supporté dans EntityData: "
                      << static_cast<int>(typeVal) << std::endl;
            return ecs::entity_t{0};
    }
}