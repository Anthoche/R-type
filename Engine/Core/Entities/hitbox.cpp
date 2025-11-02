/*
** EPITECH PROJECT, 2025
** R-type
** File description:
** hitbox
*/

#include "Include/hitbox.hpp"

namespace game::entities {

ecs::entity_t create_hitbox_for(ecs::registry &reg, ecs::entity_t owner) {
    auto hitbox = reg.spawn_entity();

    component::position pos{0.f, 0.f, 0.f};
    auto &positionsAll = reg.get_components<component::position>();
    if (owner.value() < positionsAll.size() && positionsAll[owner.value()]) {
        pos = *positionsAll[owner.value()];
    }
    reg.add_component<component::position>(hitbox, std::move(pos));

    component::collision_box box{32.f, 32.f, 32.f};
    auto &drawables = reg.get_components<component::drawable>();
    if (owner.value() < drawables.size() && drawables[owner.value()]) {
        box.width = drawables[owner.value()]->width;
        box.height = drawables[owner.value()]->height;
        box.depth = drawables[owner.value()]->depth;
    }
    reg.add_component<component::collision_box>(hitbox, std::move(box));

    reg.emplace_component<component::hitbox_link>(hitbox, owner, 0.f, 0.f, 0.f);

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
        for (std::size_t i = 0; i < hitPos.size() && i < hitBox.size() && i < link.size(); ++i) {
            if (!hitPos[i] || !hitBox[i] || !link[i])
                continue;

            ecs::entity_t owner = link[i]->owner;
            if (owner.value() >= positionsAll.size() || !positionsAll[owner.value()])
                continue;

            hitPos[i]->x = positionsAll[owner.value()]->x + link[i]->offsetX;
            hitPos[i]->y = positionsAll[owner.value()]->y + link[i]->offsetY;
            hitPos[i]->z = positionsAll[owner.value()]->z + link[i]->offsetZ;
        }
    });
}

}
