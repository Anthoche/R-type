/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** image
*/

#include "Include/image.hpp"

namespace game::entities {

ecs::entity_t create_image(ecs::registry &reg, Texture2D texture, float x, float y, float z,
    float width, float height, float depth)
{
    auto e = reg.spawn_entity();

    reg.emplace_component<component::position>(e, x, y, z);

    component::drawable drawable;
    drawable.width = width;
    drawable.height = height;
    drawable.depth = depth;
    reg.add_component<component::drawable>(e, std::move(drawable));

    component::sprite sprite;
    sprite.texture = texture;
    sprite.image_path = "";
    sprite.scale = 1.f;
    reg.add_component<component::sprite>(e, std::move(sprite));

    reg.emplace_component<component::type>(e, component::entity_type::IMAGE);
    return e;
}

ecs::entity_t create_image(ecs::registry &reg, Texture2D texture, DynamicPosition pos,
    Vector3 offset, float width, float height, float depth)
{
    auto e = reg.spawn_entity();

    reg.emplace_component<component::dynamic_position>(e, pos, offset.x, offset.y, offset.z);

    component::drawable drawable;
    drawable.width = (width < 0.f ? texture.width : width);
    drawable.height = (height < 0.f ? texture.height : height);
    drawable.depth = depth;
    reg.add_component<component::drawable>(e, std::move(drawable));

    component::sprite sprite;
    sprite.texture = texture;
    sprite.image_path = "";
    sprite.scale = 1.f;
    reg.add_component<component::sprite>(e, std::move(sprite));

    reg.emplace_component<component::type>(e, component::entity_type::IMAGE);
    return e;
}

} // namespace game::entities
