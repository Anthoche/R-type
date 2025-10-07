cmake_minimum_required(VERSION 3.5)

# -------------------------
# Project setup
# -------------------------
set(OUTPUT UnitTests)
set(ENGINE_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Core)
set(ENGINE_PHYSICS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Physics)
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../Shared)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(${OUTPUT} LANGUAGES CXX)

# -------------------------
# Source files to be tested
# -------------------------
set(SRC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
    ${ENGINE_CORE_DIR}/Entities/background.cpp
    ${ENGINE_CORE_DIR}/Entities/button.cpp
    ${ENGINE_CORE_DIR}/Entities/enemy.cpp
    ${ENGINE_CORE_DIR}/Entities/hitbox.cpp
    ${ENGINE_CORE_DIR}/Entities/obstacle.cpp
    ${ENGINE_CORE_DIR}/Entities/player.cpp
    ${ENGINE_CORE_DIR}/Entities/projectile.cpp
    ${ENGINE_CORE_DIR}/Entities/random_element.cpp
    ${ENGINE_CORE_DIR}/Entities/sound.cpp
    ${ENGINE_CORE_DIR}/Entities/text.cpp
    ${ENGINE_PHYSICS_DIR}/Collision.cpp
)

# -------------------------
# Header files
# -------------------------
set(HEAD
    ${ENGINE_CORE_DIR}/Include/entity.hpp
    ${ENGINE_CORE_DIR}/Include/registry.hpp
    ${ENGINE_CORE_DIR}/Include/sparse_array.hpp
    ${ENGINE_CORE_DIR}/Include/zipper.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/background.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/button.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/enemy.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/hitbox.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/obstacle.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/player.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/projectile.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/random_element.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/sound.hpp
    ${ENGINE_CORE_DIR}/Entities/Include/text.hpp
    ${ENGINE_PHYSICS_DIR}/Include/Collision.hpp
)

# -------------------------
# Unit test source files
# -------------------------
set(TESTS
    Engine/Core/ArrayTests.cpp
    Engine/Core/EntityTests.cpp
    Engine/Core/RegistryTests.cpp
    Engine/Core/ZipperTests.cpp
    Engine/Core/entities/BackgroundTests.cpp
    Engine/Core/entities/ButtonTests.cpp
    Engine/Core/entities/EnemyTests.cpp
    Engine/Core/entities/HitboxTests.cpp
    Engine/Core/entities/ObstacleTests.cpp
    Engine/Core/entities/PlayerTests.cpp
    Engine/Core/entities/ProjectileTests.cpp
    Engine/Core/entities/Random_elementTests.cpp
    Engine/Core/entities/SoundTests.cpp
    Engine/Core/entities/TextTests.cpp
    Engine/Physics/CollisionTests.cpp
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
    ${ENGINE_CORE_DIR}/Entities/Include
    ${SHARED_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Rendering
    ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Scene/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../Engine/Physics/Include
)

# -------------------------
# Link with Criterion
# -------------------------
target_link_libraries(${OUTPUT}
    criterion asio
)

# -------------------------
# Enable CTest integration
# -------------------------
enable_testing()
add_test(NAME ${OUTPUT} COMMAND ${OUTPUT})
