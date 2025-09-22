/*
** EPITECH PROJECT, 2025
** G-CPP-500-PAR-5-1-rtype-1
** File description:
** hitbox
*/

#include "Include/hitbox.hpp"

namespace game::entities {

static void copy_dimensions_from_owner(ecs::registry &reg, ecs::entity_t owner, component::collision_box &outBox) {
    auto &boxes = reg.get_components<component::collision_box>();
    auto &drawables = reg.get_components<component::drawable>();
    std::size_t idx = owner.value();
    if (idx < boxes.size() && boxes[idx]) {
        outBox.width = boxes[idx]->width;
        outBox.height = boxes[idx]->height;
        return;
    }
    if (idx < drawables.size() && drawables[idx]) {
        outBox.width = drawables[idx]->width;
        outBox.height = drawables[idx]->height;
        return;
    }
}

ecs::entity_t create_hitbox_for(ecs::registry &reg, ecs::entity_t owner) {
    auto hitbox = reg.spawn_entity();

    component::position pos{0.f, 0.f};
    auto &positions = reg.get_components<component::position>();
    std::size_t idx = owner.value();
    if (idx < positions.size() && positions[idx]) {
        pos.x = positions[idx]->x;
        pos.y = positions[idx]->y;
    }
    reg.add_component<component::position>(hitbox, std::move(pos));

    component::collision_box box{32.f, 32.f};
    copy_dimensions_from_owner(reg, owner, box);
    reg.add_component<component::collision_box>(hitbox, std::move(box));

    reg.emplace_component<component::hitbox_link>(hitbox, owner, 0.f, 0.f);

    return hitbox;
}

void setup_hitbox_sync_system(ecs::registry &reg) {
    reg.add_system<component::position, component::collision_box, component::hitbox_link>(
        [](ecs::registry &reg,
           ecs::sparse_array<component::position> &pos,
           ecs::sparse_array<component::collision_box> &box,
           ecs::sparse_array<component::hitbox_link> &link) {
            for (std::size_t i = 0; i < pos.size() && i < box.size() && i < link.size(); ++i) {
                if (!pos[i] || !box[i] || !link[i]) continue;
                ecs::entity_t owner = link[i]->owner;
                auto &positions = reg.get_components<component::position>();
                std::size_t idx = owner.value();
                if (idx < positions.size() && positions[idx]) {
                    pos[i]->x = positions[idx]->x + link[i]->offsetX;
                    pos[i]->y = positions[idx]->y + link[i]->offsetY;
                }
                component::collision_box ownerBox{box[i]->width, box[i]->height};
                copy_dimensions_from_owner(reg, owner, ownerBox);
                box[i]->width = ownerBox.width;
                box[i]->height = ownerBox.height;
            }
        });
}

}


