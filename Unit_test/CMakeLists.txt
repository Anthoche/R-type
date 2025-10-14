cmake_minimum_required(VERSION 3.5)

# -------------------------
# Project setup
# -------------------------
set(OUTPUT UnitTests)
set(ENGINE_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Core)
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Shared)
set(ENTITIES_DIR ${ENGINE_CORE_DIR}/Entities)

project(${OUTPUT} LANGUAGES CXX)

# -------------------------
# Enable testing early
# -------------------------
enable_testing()

# -------------------------
# Find GoogleTest (installed via vcpkg)
# -------------------------
find_package(GTest CONFIG REQUIRED)

# -------------------------
# Source files to be tested
# -------------------------
set(SRC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
    ${ENTITIES_DIR}/background.cpp
    ${ENTITIES_DIR}/button.cpp
    ${ENTITIES_DIR}/enemy.cpp
    ${ENTITIES_DIR}/hitbox.cpp
    ${ENTITIES_DIR}/image.cpp
    ${ENTITIES_DIR}/obstacle.cpp
    ${ENTITIES_DIR}/player.cpp
    ${ENTITIES_DIR}/projectile.cpp
    ${ENTITIES_DIR}/random_element.cpp
    ${ENTITIES_DIR}/sound.cpp
    ${ENTITIES_DIR}/text.cpp
)

# -------------------------
# Header files
# -------------------------
set(HEAD
    ${ENGINE_CORE_DIR}/Include/entity.hpp
    ${ENGINE_CORE_DIR}/Include/registry.hpp
    ${ENGINE_CORE_DIR}/Include/sparse_array.hpp
    ${ENGINE_CORE_DIR}/Include/zipper.hpp
    ${ENTITIES_DIR}/Include/background.hpp
    ${ENTITIES_DIR}/Include/button.hpp
    ${ENTITIES_DIR}/Include/components.hpp
    ${ENTITIES_DIR}/Include/DynamicPosition.hpp
    ${ENTITIES_DIR}/Include/enemy.hpp
    ${ENTITIES_DIR}/Include/hitbox.hpp
    ${ENTITIES_DIR}/Include/image.hpp
    ${ENTITIES_DIR}/Include/obstacle.hpp
    ${ENTITIES_DIR}/Include/player.hpp
    ${ENTITIES_DIR}/Include/projectile.hpp
    ${ENTITIES_DIR}/Include/random_element.hpp
    ${ENTITIES_DIR}/Include/sound.hpp
    ${ENTITIES_DIR}/Include/text.hpp

)

# -------------------------
# Unit test source files
# -------------------------
set(TESTS
    Engine/Core/ArrayTests.cpp
    Engine/Core/EntityTests.cpp
    Engine/Core/RegistryTests.cpp
    Engine/Core/ZipperTests.cpp
    Engine/Core/entities/PlayerTests.cpp
    Engine/Core/entities/EnemyTests.cpp
    Engine/Core/entities/ProjectileTests.cpp
    Engine/Core/entities/HitboxTests.cpp
    Engine/Core/entities/BackgroundTests.cpp
    Engine/Core/entities/ButtonTests.cpp
    Engine/Core/entities/ObstacleTests.cpp
    Engine/Core/entities/SoundTests.cpp
    Engine/Core/entities/TextTests.cpp
    Engine/Core/entities/Random_elementTests.cpp
)

# -------------------------
# Build the UnitTests executable
# -------------------------
add_executable(${OUTPUT} ${SRC} ${HEAD} ${TESTS})

# -------------------------
# Include directories
# -------------------------
target_include_directories(${OUTPUT} PRIVATE
    ${ENGINE_CORE_DIR}/Include
    ${ENTITIES_DIR}/Include
    ${SHARED_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# -------------------------
# Link with Criterion
# -------------------------
target_link_libraries(${OUTPUT}
    GTest::gtest
    GTest::gtest_main
)

# -------------------------
# Enable CTest integration
# -------------------------
include(GoogleTest)
gtest_discover_tests(${OUTPUT}
    TEST_PREFIX "UnitTests."
)
