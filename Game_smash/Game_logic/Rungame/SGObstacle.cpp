/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGObstacle
*/

#include "../Rungame.hpp"
#include "../../../Shared/Logger.hpp"
#include <cstring>

static uint32_t to_network_bits(float value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(float));
    return htonl(bits);
}

void ServerGame::initialize_obstacles() {}

bool ServerGame::is_position_blocked(float testX, float testY, float playerWidth, float playerHeight,
                                     const std::vector<ecs::entity_t> &obstacles)
{
    for (auto entity : obstacles) {
        auto pos = component_ptr<component::position>(registry_server, entity);
        auto box = component_ptr<component::collision_box>(registry_server, entity);
        if (!pos || !box)
            continue;
        float halfPlayerWidth = playerWidth * 0.5f;
        float halfPlayerHeight = playerHeight * 0.5f;
        float halfObstacleWidth = box->width * 0.5f;
        float halfObstacleHeight = box->height * 0.5f;
        float pLeft = testX - halfPlayerWidth;
        float pRight = testX + halfPlayerWidth;
        float pTop = testY - halfPlayerHeight;
        float pBottom = testY + halfPlayerHeight;
        float oLeft = pos->x - halfObstacleWidth;
        float oRight = pos->x + halfObstacleWidth;
        float oTop = pos->y - halfObstacleHeight;
        float oBottom = pos->y + halfObstacleHeight;
        if (check_aabb_overlap(pLeft, pRight, pTop, pBottom, oLeft, oRight, oTop, oBottom))
            return true;
    }
    return false;
}

bool ServerGame::is_position_blocked_platform(uint32_t clientId, float testX, float testY, float playerWidth, float playerHeight,
                                              const std::vector<ecs::entity_t> &platforms, bool movingDown, bool isDownPressed)
{
    auto currentIt = playerPositions.find(clientId);
    float currentY = currentIt != playerPositions.end() ? currentIt->second.second : testY;
    const float epsilon = 5.f;
    for (auto entity : platforms) {
        auto pos = component_ptr<component::position>(registry_server, entity);
        auto box = component_ptr<component::collision_box>(registry_server, entity);
        if (!pos || !box)
            continue;
        float halfPlayerWidth = playerWidth * 0.5f;
        float halfPlayerHeight = playerHeight * 0.5f;
        float pLeft = testX - halfPlayerWidth;
        float pRight = testX + halfPlayerWidth;
        float oHalfWidth = box->width * 0.5f;
        float oLeft = pos->x - oHalfWidth;
        float oRight = pos->x + oHalfWidth;
        if (pRight <= oLeft || pLeft >= oRight)
            continue;
        float platformTop = pos->y - box->height * 0.5f;
        float prevBottom = currentY + halfPlayerHeight;
        float pBottom = testY + halfPlayerHeight;
        bool crossing = prevBottom <= platformTop + epsilon && pBottom >= platformTop;
        if (movingDown && crossing && !isDownPressed)
            return true;
    }
    return false;
}

void ServerGame::broadcast_obstacle_spawn(uint32_t obstacleId, float x, float y, float z, float w, float h, float d)
{
    ObstacleSpawnMessage msg{};
    msg.type = MessageType::ObstacleSpawn;
    msg.obstacleId = htonl(obstacleId);
    msg.pos.xBits = to_network_bits(x);
    msg.pos.yBits = to_network_bits(y);
    msg.pos.zBits = to_network_bits(z);
    msg.size.widthBits = to_network_bits(w);
    msg.size.heightBits = to_network_bits(h);
    msg.size.depthBits = to_network_bits(d);
    connexion.broadcast(&msg, sizeof(msg));
    LOG_DEBUG("[Server] Broadcast obstacle spawn: ID=" << obstacleId
              << " pos=(" << x << "," << y << "," << z << ") size=(" << w << "," << h << "," << d << ")");
}
