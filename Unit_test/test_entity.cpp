#include <criterion/criterion.h>
#include "entity.hpp"

Test(entity, create_and_compare) {
    ecs::entity_t e1(1);
    ecs::entity_t e2(1);
    ecs::entity_t e3(2);

    cr_assert_eq(e1, e2, "should be equal");
    cr_assert_neq(e1, e3, "should not be equal");
}
