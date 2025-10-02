cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# -------------------------
# Dependencies
# -------------------------
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# -------------------------
# Directories
# -------------------------
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Client)
set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Server)
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shared)
set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine)
set(ENGINE_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core)
set(GAME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Game)

# -------------------------
# ECS library (Engine/Core)
# -------------------------
add_library(ecs STATIC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
)

target_include_directories(ecs PUBLIC
    ${ENGINE_CORE_DIR}/Include
    ${SHARED_DIR}
)

# -------------------------
# r-type_client (with Raylib)
# -------------------------
add_executable(r-type_client
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
    ${ENGINE_DIR}/Rendering/Raylib.cpp
    ${ENGINE_DIR}/Rendering/scene/AScene.cpp
    ${ENGINE_DIR}/Rendering/scene/SceneHandler.cpp
    ${CLIENT_DIR}/main.cpp
    ${CLIENT_DIR}/Network_client/client.cpp
    ${GAME_DIR}/Game.cpp
    ${GAME_DIR}/Entities/player.cpp
    ${GAME_DIR}/Entities/enemy.cpp
    ${GAME_DIR}/Entities/hitbox.cpp
    ${GAME_DIR}/Entities/obstacle.cpp
    ${GAME_DIR}/Entities/background.cpp
    ${GAME_DIR}/Entities/sound.cpp
    ${GAME_DIR}/Entities/text.cpp
    ${GAME_DIR}/Entities/button.cpp
    ${GAME_DIR}/Entities/random_element.cpp
    ${GAME_DIR}/Scene/GameScene.cpp
    ${GAME_DIR}/Scene/MenuScene.cpp
    ${GAME_DIR}/Scene/SettingsScene.cpp
    ${GAME_DIR}/Système/Collision.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CLIENT_DIR}/Network_client/Include
    ${CLIENT_DIR}/Input
    ${GAME_DIR}
    ${GAME_DIR}/Entities/Include
    ${GAME_DIR}/Scene
    ${SHARED_DIR}
)

target_link_libraries(r-type_client PRIVATE ecs raylib glfw)

# -------------------------
# r-type_server (no Raylib)
# -------------------------
add_executable(r-type_server
    ${SERVER_DIR}/main.cpp
    ${SERVER_DIR}/server.cpp
    ${SERVER_DIR}/Network_handler/connexion.cpp
    ${SERVER_DIR}/Network_handler/UDP_socket.cpp
    ${SERVER_DIR}/Game_logic/game_setup.cpp
    ${GAME_DIR}/ServerGame.cpp
)

target_include_directories(r-type_server PRIVATE
    ${SERVER_DIR}/Network_handler/Include
    ${SERVER_DIR}/Game_logic/Include
    ${GAME_DIR}
    ${SHARED_DIR}
)

target_link_libraries(r-type_server PRIVATE ecs)

# -------------------------
# Unit tests
# -------------------------
enable_testing()
add_subdirectory(Unit_test)

# -------------------------
# Installation
# -------------------------
install(TARGETS r-type_client r-type_server ecs
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# -------------------------
# Assets setup
# -------------------------
execute_process(
    # COMMAND ${CMAKE_COMMAND} -E tar xzf ${CMAKE_SOURCE_DIR}/assets.zip # /!\ DO NOT DELETE, to be used after
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Game/Assets/ ./assets # Temporary solution to get access to assets directory
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_compile_definitions(r-type_client PUBLIC ASSETS_PATH="${CMAKE_BINARY_DIR}/assets")
