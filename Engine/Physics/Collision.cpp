/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** Collision
*/

#include "../../../Game/Scene/Include/GameScene.hpp"
#include <iostream>
#include <cmath>

namespace game::scene::collision {

    ecs::entity_t find_player_hitbox(GameScene &scene) {
        auto &links = scene.get_registry().get_components<component::hitbox_link>();
        for (std::size_t i = 0; i < links.size(); ++i) {
            if (links[i] && links[i]->owner == scene.get_player()) {
                return scene.get_registry().entity_from_index(i);
            }
        }
        return ecs::entity_t{0};
    }

    bool is_blocked(GameScene &scene, float testX, float testY,
                    const component::position &playerPos,
                    const component::collision_box &playerBox) {
        float left   = testX - playerBox.width * 0.5f;
        float right  = testX + playerBox.width * 0.5f;
        float top    = testY - playerBox.height * 0.5f;
        float bottom = testY + playerBox.height * 0.5f;
        auto &positions = scene.get_registry().get_components<component::position>();
        auto &hitboxes  = scene.get_registry().get_components<component::collision_box>();

        for (auto &obstacle : scene.get_obstacles()) {
            if (obstacle.value() >= positions.size() || !positions[obstacle.value()] || !hitboxes[obstacle.value()])
                continue;
            auto &obsPos = *positions[obstacle.value()];
            auto &obsBox = *hitboxes[obstacle.value()];
            float obsLeft   = obsPos.x - obsBox.width * 0.5f;
            float obsRight  = obsPos.x + obsBox.width * 0.5f;
            float obsTop    = obsPos.y - obsBox.height * 0.5f;
            float obsBottom = obsPos.y + obsBox.height * 0.5f;
            bool overlap = right > obsLeft && left < obsRight && bottom > obsTop && top < obsBottom;
            if (overlap)
                return true;
        }
        return false;
    }

    ecs::entity_t find_hitbox_of(GameScene &scene, ecs::entity_t owner) {
        auto &links = scene.get_registry().get_components<component::hitbox_link>();
        for (std::size_t i = 0; i < links.size(); ++i) {
            if (links[i] && links[i]->owner == owner) {
                return scene.get_registry().entity_from_index(i);
            }
        }
        return ecs::entity_t{0};
    }

    bool overlap_aabb(float leftA, float rightA, float topA, float bottomA,
                    float leftB, float rightB, float topB, float bottomB) {
        return rightA > leftB && leftA < rightB && bottomA > topB && topA < bottomB;
    }

    void resolve_collision(GameScene &scene,
                        ecs::entity_t playerEntity,
                        component::position &playerPos,
                        component::collision_box &playerBox,
                        const component::position &obsPos,
                        const component::collision_box &obsBox,
                        float prevX, float prevY) {
        auto &velocitiesAll  = scene.get_registry().get_components<component::velocity>();
        auto &prevPositions  = scene.get_registry().get_components<component::previous_position>();
        float obsLeft   = obsPos.x - obsBox.width * 0.5f;
        float obsRight  = obsPos.x + obsBox.width * 0.5f;
        float obsTop    = obsPos.y - obsBox.height * 0.5f;
        float obsBottom = obsPos.y + obsBox.height * 0.5f;
        float moveX = playerPos.x - prevX;
        float moveY = playerPos.y - prevY;
        const float skin = 0.5f;

        if (std::fabs(moveX) > std::fabs(moveY)) {
            if (moveX > 0.f)
                playerPos.x = obsLeft - playerBox.width * 0.5f - skin;
            else
                playerPos.x = obsRight + playerBox.width * 0.5f + skin;
            if (playerEntity.value() < velocitiesAll.size() && velocitiesAll[playerEntity.value()])
                velocitiesAll[playerEntity.value()]->vx = 0.f;
        } else {
            if (moveY > 0.f)
                playerPos.y = obsTop - playerBox.height * 0.5f - skin;
            else
                playerPos.y = obsBottom + playerBox.height * 0.5f + skin;
            if (playerEntity.value() < velocitiesAll.size() && velocitiesAll[playerEntity.value()])
                velocitiesAll[playerEntity.value()]->vy = 0.f;
        }
        if (playerEntity.value() < prevPositions.size() && prevPositions[playerEntity.value()]) {
            prevPositions[playerEntity.value()]->x = playerPos.x;
            prevPositions[playerEntity.value()]->y = playerPos.y;
        }
    }

    void handle_entity_collisions(GameScene &scene, ecs::entity_t entity) {
        auto &positions     = scene.get_registry().get_components<component::position>();
        auto &hitboxes      = scene.get_registry().get_components<component::collision_box>();
        auto &prevPositions = scene.get_registry().get_components<component::previous_position>();
        auto &types         = scene.get_registry().get_components<component::type>();
        auto &healths       = scene.get_registry().get_components<component::health>();

        if (entity.value() >= positions.size() || !positions[entity.value()])
            return;
        ecs::entity_t entityHitbox = find_hitbox_of(scene, entity);
        if (!entityHitbox.value() || entityHitbox.value() >= hitboxes.size() || !hitboxes[entityHitbox.value()])
            return;
        auto &pos = *positions[entity.value()];
        auto &box = *hitboxes[entityHitbox.value()];
        float nextLeft   = pos.x - box.width * 0.5f;
        float nextRight  = pos.x + box.width * 0.5f;
        float nextTop    = pos.y - box.height * 0.5f;
        float nextBottom = pos.y + box.height * 0.5f;
        for (auto &obstacle : scene.get_obstacles()) {
            if (obstacle.value() >= positions.size() || !positions[obstacle.value()] || !hitboxes[obstacle.value()])
                continue;
            auto &obsPos = *positions[obstacle.value()];
            auto &obsBox = *hitboxes[obstacle.value()];
            float obsLeft   = obsPos.x - obsBox.width * 0.5f;
            float obsRight  = obsPos.x + obsBox.width * 0.5f;
            float obsTop    = obsPos.y - obsBox.height * 0.5f;
            float obsBottom = obsPos.y + obsBox.height * 0.5f;
            if (overlap_aabb(nextLeft, nextRight, nextTop, nextBottom,
                            obsLeft, obsRight, obsTop, obsBottom)) {
                float prevX = (entity.value() < prevPositions.size() && prevPositions[entity.value()])
                                ? prevPositions[entity.value()]->x
                                : pos.x;
                float prevY = (entity.value() < prevPositions.size() && prevPositions[entity.value()])
                                ? prevPositions[entity.value()]->y
                                : pos.y;
                resolve_collision(scene, entity, pos, box, obsPos, obsBox, prevX, prevY);
            }
        }
        for (std::size_t i = 0; i < types.size(); ++i) {
            if (!types[i] || types[i]->value != component::entity_type::ENEMY)
                continue;
            ecs::entity_t enemy = scene.get_registry().entity_from_index(i);
            ecs::entity_t enemyHitbox = find_hitbox_of(scene, enemy);
            if (!enemyHitbox.value() || enemyHitbox.value() >= hitboxes.size() || !hitboxes[enemyHitbox.value()])
                continue;
            auto &enemyPos = *positions[enemy.value()];
            auto &enemyBox = *hitboxes[enemyHitbox.value()];
            float enemyLeft   = enemyPos.x - enemyBox.width * 0.5f;
            float enemyRight  = enemyPos.x + enemyBox.width * 0.5f;
            float enemyTop    = enemyPos.y - enemyBox.height * 0.5f;
            float enemyBottom = enemyPos.y + enemyBox.height * 0.5f;
            if (overlap_aabb(nextLeft, nextRight, nextTop, nextBottom,
                            enemyLeft, enemyRight, enemyTop, enemyBottom)) {
                if (entity.value() < healths.size() && healths[entity.value()]) {
                    healths[entity.value()]->current = 0;
                }
            }
        }
    }
} // namespace game::scene::collision
