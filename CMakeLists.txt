cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
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
find_package(nlohmann_json CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)

# -------------------------
# Directories
# -------------------------
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Client)
set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Server)
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shared)
set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine)
set(ENGINE_CORE_DIR ${ENGINE_DIR}/Core)
set(ENGINE_ENTITIES_DIR ${ENGINE_CORE_DIR}/Entities)
set(ENGINE_RENDERING_DIR ${ENGINE_DIR}/Rendering)
set(ENGINE_SCENE_DIR ${ENGINE_RENDERING_DIR}/scene)
set(ENGINE_PHYSICS_DIR ${ENGINE_DIR}/Physics)
set(ENGINE_UTILS_DIR ${ENGINE_DIR}/Utils)

# -------------------------
# ECS library (Engine/Core)
# -------------------------
add_library(ecs STATIC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
    ${ENGINE_CORE_DIR}/Entities/Include/button.hpp
)

target_include_directories(ecs PUBLIC
    ${ENGINE_CORE_DIR}/Include
    ${SHARED_DIR}
)

# -------------------------
# r-type_client (with Raylib)
# -------------------------
add_executable(r-type_client
    ${CLIENT_DIR}/main.cpp
    ${CLIENT_DIR}/client.cpp
    ${CLIENT_DIR}/Network_client/UDP_socket.cpp
    ${CLIENT_DIR}/Network_client/TCP_socketClient.cpp
    ${CLIENT_DIR}/Handler/client_handler.cpp
    ${ENGINE_DIR}/Game.cpp
    ${ENGINE_ENTITIES_DIR}/player.cpp
    ${ENGINE_ENTITIES_DIR}/enemy.cpp
    ${ENGINE_ENTITIES_DIR}/hitbox.cpp
    ${ENGINE_ENTITIES_DIR}/obstacle.cpp
    ${ENGINE_ENTITIES_DIR}/background.cpp
    ${ENGINE_ENTITIES_DIR}/sound.cpp
    ${ENGINE_ENTITIES_DIR}/text.cpp
    ${ENGINE_ENTITIES_DIR}/projectile.cpp
    ${ENGINE_ENTITIES_DIR}/button.cpp
    ${ENGINE_ENTITIES_DIR}/random_element.cpp
    ${ENGINE_UTILS_DIR}/entity_parser.cpp
    ${ENGINE_UTILS_DIR}/entity_storage.cpp
    ${ENGINE_UTILS_DIR}/serializer.cpp
    ${ENGINE_RENDERING_DIR}/Raylib.cpp
    ${ENGINE_RENDERING_DIR}/RenderUtils.cpp
    ${ENGINE_SCENE_DIR}/AScene.cpp
    ${ENGINE_SCENE_DIR}/SceneHandler.cpp
    ${ENGINE_DIR}/Scene/GameScene.cpp
    ${ENGINE_DIR}/Scene/MenuScene.cpp
    ${ENGINE_DIR}/Scene/SettingsScene.cpp
    ${ENGINE_DIR}/Scene/WaitingScene.cpp
    ${ENGINE_DIR}/Scene/ServerWaitScene.cpp
    ${ENGINE_PHYSICS_DIR}/Collision.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CLIENT_DIR}
    ${CLIENT_DIR}/Handler
    ${CLIENT_DIR}/Network_client
    ${ENGINE_CORE_DIR}/Include
    ${ENGINE_ENTITIES_DIR}/Include
    ${ENGINE_RENDERING_DIR}
    ${ENGINE_SCENE_DIR}
    ${ENGINE_DIR}/Scene
    ${ENGINE_UTILS_DIR}/Include
    ${SHARED_DIR}
    ${ASIO_INCLUDE_DIR}
)

target_link_libraries(r-type_client PRIVATE ecs raylib glfw nlohmann_json asio)

if (WIN32)
    target_compile_definitions(r-type_client PRIVATE WIN32_LEAN_AND_MEAN NOGDI NOUSER)
endif()

# -------------------------
# r-type_server (no Raylib)
# -------------------------
add_executable(r-type_server
    ${SERVER_DIR}/main.cpp
    ${SERVER_DIR}/server.cpp
    ${SERVER_DIR}/Network_handler/connexion.cpp
    ${SERVER_DIR}/Network_handler/UDP_socket.cpp
    ${SERVER_DIR}/Network_handler/TCP_socketServer.cpp
    ${SERVER_DIR}/Game_logic/game_setup.cpp
    ${SERVER_DIR}/Game_logic/ServerGame.cpp
    ${SERVER_DIR}/Game_logic/ServerGame/SGPlayer.cpp
    ${SERVER_DIR}/Game_logic/ServerGame/SGEnnemy.cpp
    ${SERVER_DIR}/Game_logic/ServerGame/SGObstacle.cpp
    ${SERVER_DIR}/Game_logic/ServerGame/SGProjectile.cpp
    ${ENGINE_UTILS_DIR}/entity_parser.cpp
    ${ENGINE_UTILS_DIR}/entity_storage.cpp
    ${ENGINE_UTILS_DIR}/serializer.cpp
    ${ENGINE_ENTITIES_DIR}/player.cpp
    ${ENGINE_ENTITIES_DIR}/enemy.cpp
    ${ENGINE_ENTITIES_DIR}/obstacle.cpp
    ${ENGINE_ENTITIES_DIR}/background.cpp
    ${ENGINE_ENTITIES_DIR}/sound.cpp
    ${ENGINE_ENTITIES_DIR}/text.cpp
    ${ENGINE_ENTITIES_DIR}/random_element.cpp
    ${ENGINE_ENTITIES_DIR}/hitbox.cpp
)

target_include_directories(r-type_server PRIVATE
    ${SERVER_DIR}/Network_handler/Include
    ${SERVER_DIR}/Game_logic/Include
    ${ENGINE_CORE_DIR}/Include
    ${ENGINE_ENTITIES_DIR}/Include
    ${ENGINE_UTILS_DIR}/Include
    ${SHARED_DIR}
    ${ASIO_INCLUDE_DIR}
)
target_link_libraries(r-type_server PRIVATE ecs nlohmann_json)

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
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Engine/Assets/ ./assets # Temporary solution to get access to assets directory
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_compile_definitions(r-type_client PUBLIC ASSETS_PATH="${CMAKE_BINARY_DIR}/assets")