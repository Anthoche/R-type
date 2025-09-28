/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** hitbox
*/

#include "Include/hitbox.hpp"
#include <iostream>

namespace game::entities {

    ecs::entity_t create_hitbox_for(ecs::registry &reg, ecs::entity_t owner) {
        auto hitbox = reg.spawn_entity();

        component::position initialPos{0.f, 0.f};
        auto &allPos = reg.get_components<component::position>();
        if (owner.value() < allPos.size() && allPos[owner.value()]) {
            initialPos.x = allPos[owner.value()]->x;
            initialPos.y = allPos[owner.value()]->y;
        }
        reg.add_component<component::position>(hitbox, std::move(initialPos));

        component::collision_box box{32.f, 32.f};
        auto &boxes = reg.get_components<component::collision_box>();
        auto &drawables = reg.get_components<component::drawable>();
        std::size_t idx = owner.value();
        if (idx < boxes.size() && boxes[idx])
            box.width = boxes[idx]->width, box.height = boxes[idx]->height;
        else if (idx < drawables.size() && drawables[idx])
            box.width = drawables[idx]->width, box.height = drawables[idx]->height;

        reg.add_component<component::collision_box>(hitbox, std::move(box));
        reg.emplace_component<component::hitbox_link>(hitbox, owner, 0.f, 0.f);
        return hitbox;
    }

    void setup_hitbox_sync_system(ecs::registry &reg) {
        reg.add_system<component::position, component::collision_box, component::hitbox_link>(
        [](ecs::registry &reg,
        ecs::sparse_array<component::position> &hitPos,
        ecs::sparse_array<component::collision_box> &hitBox,
        ecs::sparse_array<component::hitbox_link> &link)
        {
            auto &positionsAll = reg.get_components<component::position>();
            auto &boxesAll = reg.get_components<component::collision_box>();

            for (std::size_t i = 0; i < hitPos.size() && i < hitBox.size() && i < link.size(); ++i) {
                if (!hitPos[i] || !hitBox[i] || !link[i])
                    continue;
                ecs::entity_t owner = link[i]->owner;
                std::size_t ownerIndex = static_cast<std::size_t>(-1);
                for (std::size_t j = 0; j < positionsAll.size(); ++j) {
                    if (positionsAll[j] && reg.entity_from_index(j) == owner) {
                        ownerIndex = j;
                        break;
                    }
                }
                if (ownerIndex == static_cast<std::size_t>(-1))
                    continue;
                hitPos[i]->x = positionsAll[ownerIndex]->x + link[i]->offsetX;
                hitPos[i]->y = positionsAll[ownerIndex]->y + link[i]->offsetY;
                if (ownerIndex < boxesAll.size() && boxesAll[ownerIndex]) {
                    hitBox[i]->width = boxesAll[ownerIndex]->width;
                    hitBox[i]->height = boxesAll[ownerIndex]->height;
                }
            }
        });
    }

} // namespace game::entities
