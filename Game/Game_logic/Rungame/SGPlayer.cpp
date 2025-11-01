/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** SGPlayer
*/

#include "../Rungame.hpp"
#include "../../../Shared/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>


void ServerGame::initialize_player_positions() {
    LOG_DEBUG("Initializing player positions...");
    for (const auto& kv : connexion.getClients()) {
        float spawnX = 300.f + (kv.second - 1) * 100.f;
        float spawnY = 100.f;
        playerPositions[kv.second] = {spawnX, spawnY};
        playerSpawnPositions[kv.second] = {spawnX, spawnY};
        ensure_player_tracking(kv.second);
        playerLives[kv.second] = 3;
        playerVelocities[kv.second] = 0.f;
        playerHorizontalKnockback[kv.second] = 0.f;
        playerVerticalKnockback[kv.second] = 0.f;
        playerJumpCount[kv.second] = 0;
        playerInputStates[kv.second] = PlayerInputState{};
        deadPlayers.erase(kv.second);
    }
}

void ServerGame::handle_melee_attack(uint32_t attackerId, float range, int damage, float baseKnockback, float knockbackScale, int dirX, int dirY) {
    auto attackerPosIt = playerPositions.find(attackerId);
    if (attackerPosIt == playerPositions.end())
        return;
    if (dirX == 0 && dirY == 0)
        dirX = 1;
    auto attackerInfo = get_player_info(attackerId);
    if (!attackerInfo)
        return;
    LOG_DEBUG("[Server] Player " << attackerId << " performs melee attack (range=" << range
              << ", damage=" << damage << ")");
    auto target = select_melee_target(attackerId, range, dirX, dirY, *attackerInfo,
                                      attackerPosIt->second.first, attackerPosIt->second.second);
    if (!target)
        return;
    apply_melee_damage(attackerId, *target, damage, baseKnockback, knockbackScale, dirX, dirY);
}

bool ServerGame::is_on_ground(uint32_t clientId, float x, float y, float playerWidth, float playerHeight) {
    constexpr float checkDistance = 2.f;
    float testY = y + checkDistance;
    
    return is_position_blocked(x, testY, playerWidth, playerHeight, _obstacles) ||
           is_position_blocked_platform(clientId, x, testY, playerWidth, playerHeight, _platforms, true, false);
}

void ServerGame::apply_gravity(float dt) {
    constexpr float gravity = 800.f;
    for (auto &kv : playerPositions) {
        uint32_t clientId = kv.first;
        if (deadPlayers.find(clientId) != deadPlayers.end())
            continue;
        ensure_player_tracking(clientId);
        auto [playerWidth, playerHeight] = get_player_dimensions(clientId);
        auto &pos = kv.second;
        auto &velocity = playerVelocities[clientId];
        auto &verticalKnock = playerVerticalKnockback[clientId];
        auto &state = playerInputStates[clientId];
        auto &jumpCount = playerJumpCount[clientId];
        bool onGround = is_on_ground(clientId, pos.first, pos.second, playerWidth, playerHeight);
        if (onGround)
            jumpCount = 0;
        handle_jump_request(clientId, onGround, velocity, state, jumpCount);
        velocity += gravity * dt;
        if (std::fabs(verticalKnock) > 1.f) {
            velocity += verticalKnock;
            verticalKnock = 0.f;
        }
        integrate_vertical_motion(clientId, dt, pos, playerWidth, playerHeight, velocity, state);
        if (onGround && velocity > 0.f)
            velocity = 0.f;
    }
}


void ServerGame::respawn_player(uint32_t clientId, std::size_t registryIndex) {
    ensure_player_tracking(clientId);
    auto &drawables = registry_server.get_components<component::drawable>();
    float playerWidth = 50.f;
    float playerHeight = 50.f;
    if (registryIndex < drawables.size() && drawables[registryIndex]) {
        playerWidth = drawables[registryIndex]->width;
        playerHeight = drawables[registryIndex]->height;
    }
    auto spawn = compute_respawn_position(clientId, playerWidth, playerHeight);
    playerPositions[clientId] = spawn;
    playerSpawnPositions[clientId] = spawn;
    reset_player_motion(clientId);
    playerJumpCount[clientId] = 0;
    playerInputStates[clientId] = PlayerInputState{};
    auto &positions = registry_server.get_components<component::position>();
    if (registryIndex < positions.size() && positions[registryIndex]) {
        positions[registryIndex]->x = spawn.first;
        positions[registryIndex]->y = spawn.second;
        positions[registryIndex]->z = 0.f;
    }
    auto &prevPositions = registry_server.get_components<component::previous_position>();
    if (registryIndex < prevPositions.size() && prevPositions[registryIndex]) {
        prevPositions[registryIndex]->x = spawn.first;
        prevPositions[registryIndex]->y = spawn.second;
        prevPositions[registryIndex]->z = 0.f;
    }

    auto &healths = registry_server.get_components<component::health>();
    if (registryIndex < healths.size() && healths[registryIndex]) {
        healths[registryIndex]->current = healths[registryIndex]->max;
    }

    deadPlayers.erase(clientId);
    LOG_INFO("[Server] Player " << clientId << " respawns (" << playerLives[clientId] << " lives remaining)");
}

std::pair<float, float> ServerGame::get_player_dimensions(uint32_t clientId) {
    float width = 30.f;
    float height = 30.f;

    auto &clientIds = registry_server.get_components<component::client_id>();
    auto &collision = registry_server.get_components<component::collision_box>();
    auto &drawables = registry_server.get_components<component::drawable>();

    for (std::size_t i = 0; i < clientIds.size(); ++i) {
        if (clientIds[i] && clientIds[i]->id == clientId) {
            if (i < collision.size() && collision[i]) {
                width = collision[i]->width;
                height = collision[i]->height;
            } else if (i < drawables.size() && drawables[i]) {
                width = drawables[i]->width;
                height = drawables[i]->height;
            }
            break;
        }
    }

    return {width, height};
}


void ServerGame::ensure_player_tracking(uint32_t clientId) {
    if (playerLives.find(clientId) == playerLives.end())
        playerLives[clientId] = 3;
    if (playerVelocities.find(clientId) == playerVelocities.end())
        playerVelocities[clientId] = 0.f;
    if (playerHorizontalKnockback.find(clientId) == playerHorizontalKnockback.end())
        playerHorizontalKnockback[clientId] = 0.f;
    if (playerVerticalKnockback.find(clientId) == playerVerticalKnockback.end())
        playerVerticalKnockback[clientId] = 0.f;
    if (playerJumpCount.find(clientId) == playerJumpCount.end())
        playerJumpCount[clientId] = 0;
}

std::optional<ServerGame::PlayerCombatInfo> ServerGame::get_player_info(uint32_t clientId) {
    PlayerCombatInfo info{};
    auto &clientIds = registry_server.get_components<component::client_id>();
    auto &collision = registry_server.get_components<component::collision_box>();
    auto &drawables = registry_server.get_components<component::drawable>();
    info.index = clientIds.size();
    for (std::size_t i = 0; i < clientIds.size(); ++i) {
        if (!clientIds[i] || clientIds[i]->id != clientId)
            continue;
        info.index = i;
        if (i < collision.size() && collision[i]) {
            info.width = collision[i]->width;
            info.height = collision[i]->height;
        } else if (i < drawables.size() && drawables[i]) {
            info.width = drawables[i]->width;
            info.height = drawables[i]->height;
        }
        return info;
    }
    return std::nullopt;
}

std::optional<ServerGame::MeleeTarget> ServerGame::select_melee_target(uint32_t attackerId, float range, int dirX, int dirY, const PlayerCombatInfo &attackerInfo, float attackerX, float attackerY) {
    MeleeTarget best{};
    bool found = false;
    bool horizontalAxis = std::abs(dirX) >= std::abs(dirY);
    float axisSign = horizontalAxis ? static_cast<float>(dirX) : static_cast<float>(dirY);
    for (const auto &kv : playerPositions) {
        uint32_t targetId = kv.first;
        if (targetId == attackerId || deadPlayers.find(targetId) != deadPlayers.end())
            continue;
        float dx = kv.second.first - attackerX;
        float dy = kv.second.second - attackerY;
        float primary = horizontalAxis ? dx : dy;
        if ((axisSign > 0.f && primary < 0.f) || (axisSign < 0.f && primary > 0.f))
            continue;
        auto info = get_player_info(targetId);
        if (!info)
            continue;
        float gapX = std::max(0.f, std::fabs(dx) - (attackerInfo.width * 0.5f + info->width * 0.5f));
        float gapY = std::max(0.f, std::fabs(dy) - (attackerInfo.height * 0.5f + info->height * 0.5f));
        float distance = std::sqrt(gapX * gapX + gapY * gapY);
        if (distance > range)
            continue;
        if (!found || distance < best.distance) {
            best = {targetId, info->index, horizontalAxis, distance};
            found = true;
        }
    }
    return found ? std::optional<MeleeTarget>(best) : std::nullopt;
}

void ServerGame::apply_melee_damage(uint32_t attackerId, const MeleeTarget &target, int damage,
                                float baseKnockback, float knockbackScale, int dirX, int dirY) {
    auto &healths = registry_server.get_components<component::health>();
    if (target.index >= healths.size() || !healths[target.index])
        return;
    auto &health = *healths[target.index];
    LOG_INFO("[Server] Player " << attackerId << " hits player " << target.id
             << " at distance " << target.distance << "px for " << damage << " damage");
    health.current = std::max(0, health.current - damage);
    LOG_DEBUG("[Server] Player " << target.id << " health: " << health.current << "/" << health.max);
    float missingRatio = 0.f;
    if (health.max > 0)
        missingRatio = static_cast<float>(health.max - health.current) / static_cast<float>(health.max);
    float knockMagnitude = baseKnockback + knockbackScale * missingRatio;
    if (target.horizontal) {
        int direction = dirX >= 0 ? 1 : -1;
        playerHorizontalKnockback[target.id] = knockMagnitude * direction;
        playerVerticalKnockback[target.id] = (baseKnockback >= 650.f) ? -knockMagnitude * 0.35f : 0.f;
    } else {
        int direction = dirY >= 0 ? 1 : -1;
        playerHorizontalKnockback[target.id] = 0.f;
        playerVerticalKnockback[target.id] = knockMagnitude * 0.75f * direction;
    }
    if (health.current <= 0) {
        LOG_INFO("[Server] Player " << target.id << " was defeated by player " << attackerId);
        handle_player_defeat(target.id, target.index);
    }
}

void ServerGame::handle_jump_request(uint32_t clientId, bool onGround, float &velocity,
                                     PlayerInputState &state, int &jumpCount) {
    constexpr float jumpForce = -700.f;
    constexpr int maxJumps = 2;
    if (!state.up) {
        return;
    }
    if (!onGround && jumpCount >= maxJumps) {
        state.up = false;
        return;
    }
    velocity = jumpForce;
    if (!onGround) {
        ++jumpCount;
        LOG_DEBUG("[Server] Player " << clientId << " performed air jump " << jumpCount << "/" << maxJumps);
    }
    state.up = false;
}

void ServerGame::integrate_vertical_motion(uint32_t clientId, float dt, std::pair<float, float> &pos,
                                           float playerWidth, float playerHeight, float &velocity,
                                           PlayerInputState &state) {
    float newY = pos.second + velocity * dt;
    if (!is_position_blocked(pos.first, newY, playerWidth, playerHeight, _obstacles)) {
        bool movingDown = velocity > 0.f;
        if (!is_position_blocked_platform(clientId, pos.first, newY, playerWidth, playerHeight, _platforms, movingDown, state.down)) {
            pos.second = newY;
        } else if (movingDown) {
            pos.second = snap_to_platform_top(pos.first, pos.second, playerWidth, playerHeight);
            velocity = 0.f;
        }
    } else {
        velocity = 0.f;
    }
}
