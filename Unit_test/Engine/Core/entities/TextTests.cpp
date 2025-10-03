/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** TextTest.cpp
*/

#include <criterion/criterion.h>
#include "text.hpp"
#include "components.hpp"
#include "registry.hpp"

using namespace ecs;
using namespace game::entities;

static void register_all_text_components(registry &reg) {
    reg.register_component<component::position>();
    reg.register_component<component::text>();
    reg.register_component<component::drawable>();
    reg.register_component<component::type>();
}

Test(Text, has_position_and_text_component) {
    registry reg;
    register_all_text_components(reg);

    Vector2 pos{50.f, 100.f};
    std::string content = "Hello World";
    Color color{255, 0, 0, 255};
    int fontSize = 24;

    entity_t e = create_text(reg, pos, content, color, fontSize);

    auto &positions = reg.get_components<component::position>();
    auto &texts = reg.get_components<component::text>();
    auto &drawables = reg.get_components<component::drawable>();
    auto &types = reg.get_components<component::type>();

    cr_assert(positions[static_cast<std::size_t>(e)].has_value());
    cr_assert_eq(positions[static_cast<std::size_t>(e)]->x, 50.f);
    cr_assert_eq(positions[static_cast<std::size_t>(e)]->y, 100.f);

    cr_assert(texts[static_cast<std::size_t>(e)].has_value());
    cr_assert_str_eq(texts[static_cast<std::size_t>(e)]->content.c_str(), "Hello World");
    cr_assert_eq(texts[static_cast<std::size_t>(e)]->font_size, 24);
    cr_assert_eq(texts[static_cast<std::size_t>(e)]->color.r, 255);
    cr_assert_eq(texts[static_cast<std::size_t>(e)]->color.g, 0);
    cr_assert_eq(texts[static_cast<std::size_t>(e)]->color.b, 0);
    cr_assert_eq(texts[static_cast<std::size_t>(e)]->color.a, 255);

    cr_assert(drawables[static_cast<std::size_t>(e)].has_value());

    cr_assert(types[static_cast<std::size_t>(e)].has_value());
    cr_assert_eq(types[static_cast<std::size_t>(e)]->value, component::entity_type::TEXT);
}