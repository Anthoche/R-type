cmake_minimum_required(VERSION 3.16)

# -------------------------
# Toolchain (vcpkg support)
# -------------------------
set(VCPKG_TOOLCHAIN_CANDIDATE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
if(EXISTS "${VCPKG_TOOLCHAIN_CANDIDATE}")
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_CANDIDATE}" CACHE STRING "Vcpkg toolchain file")
endif()

project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(VCPKG_TOOLCHAIN_FALLBACK "${CMAKE_SOURCE_DIR}/external/vcpkg/scripts/buildsystems/vcpkg.cmake")
    if(EXISTS "${VCPKG_TOOLCHAIN_FALLBACK}")
        set(CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_FALLBACK}" CACHE STRING "Vcpkg toolchain file")
    endif()
endif()

# -------------------------
# Build options
# -------------------------
option(TU "Build unit tests (enable with -DTU=ON)" OFF)

# -------------------------
# Dependencies
# -------------------------
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(asio CONFIG REQUIRED)

# -------------------------
# Project directories
# -------------------------
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Client)
set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Server)
set(GAME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Game)
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shared)
set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine)
set(ENGINE_CORE_DIR ${ENGINE_DIR}/Core)
set(ENGINE_ENTITIES_DIR ${ENGINE_CORE_DIR}/Entities)
set(ENGINE_RENDERING_DIR ${ENGINE_DIR}/Rendering)
set(ENGINE_SCENE_DIR ${ENGINE_RENDERING_DIR}/scene)
set(ENGINE_PHYSICS_DIR ${ENGINE_DIR}/Physics)
set(ENGINE_UTILS_DIR ${ENGINE_DIR}/Utils)

# -------------------------
# Shared sockets library
# -------------------------
add_library(shared_sockets STATIC
    ${SHARED_DIR}/Sockets/UDP_socket.cpp
    ${SHARED_DIR}/Sockets/TCP_socket.cpp
)

target_include_directories(shared_sockets PUBLIC
    ${SHARED_DIR}/Sockets/Include
    ${SHARED_DIR}
)

target_link_libraries(shared_sockets PUBLIC
    nlohmann_json::nlohmann_json
    asio::asio
)

if(WIN32)
    target_compile_definitions(shared_sockets PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER)
endif()

# -------------------------
# ECS core
# -------------------------
add_library(ecs STATIC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
)

target_include_directories(ecs PUBLIC
    ${ENGINE_CORE_DIR}/Include
    ${ENGINE_ENTITIES_DIR}/Include
    ${SHARED_DIR}
)

if(WIN32)
    # Reduce Windows header surface and avoid macro conflicts with raylib and STL
    target_compile_definitions(ecs PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER)
endif()

# -------------------------
# Game Logic library (auto-découverte COMPLÈTE)
# -------------------------
# Recherche récursive de TOUS les .cpp dans Game/
file(GLOB_RECURSE GAME_ALL_SOURCES "${GAME_DIR}/*.cpp")

# Recherche récursive de TOUS les .hpp et .h dans Game/
file(GLOB_RECURSE GAME_ALL_HEADERS "${GAME_DIR}/*.hpp" "${GAME_DIR}/*.h")

# Extraction de tous les répertoires contenant des headers
set(GAME_INCLUDE_DIRS "")
foreach(header ${GAME_ALL_HEADERS})
    get_filename_component(dir ${header} DIRECTORY)
    list(APPEND GAME_INCLUDE_DIRS ${dir})
endforeach()
list(REMOVE_DUPLICATES GAME_INCLUDE_DIRS)

message(STATUS "=== Game library configuration ===")
message(STATUS "Game sources found: ${GAME_ALL_SOURCES}")
message(STATUS "Game include directories: ${GAME_INCLUDE_DIRS}")

add_library(game_logic STATIC ${GAME_ALL_SOURCES})

target_include_directories(game_logic PUBLIC
    ${GAME_DIR}
    ${GAME_INCLUDE_DIRS}
    ${ENGINE_CORE_DIR}/Include
    ${ENGINE_ENTITIES_DIR}/Include
    ${ENGINE_RENDERING_DIR}
    ${ENGINE_SCENE_DIR}/Include
    ${ENGINE_UTILS_DIR}/Include
    ${ENGINE_PHYSICS_DIR}/Include
    ${SHARED_DIR}
)

target_link_libraries(game_logic PUBLIC
    ecs
    nlohmann_json::nlohmann_json
)

if(WIN32)
    target_compile_definitions(game_logic PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER)
endif()

# Définir ASSETS_PATH pour game_logic
target_compile_definitions(game_logic PUBLIC ASSETS_PATH="${CMAKE_BINARY_DIR}/assets")

# -------------------------
# r-type_client executable
# -------------------------
add_executable(r-type_client
    ${CLIENT_DIR}/main.cpp
    ${CLIENT_DIR}/client.cpp
    ${CLIENT_DIR}/Handler/ClientHandler.cpp
    ${ENGINE_DIR}/Game.cpp
    ${ENGINE_ENTITIES_DIR}/background.cpp
    ${ENGINE_ENTITIES_DIR}/button.cpp
    ${ENGINE_ENTITIES_DIR}/checkpoint.cpp
    ${ENGINE_ENTITIES_DIR}/decoration.cpp
    ${ENGINE_ENTITIES_DIR}/enemy.cpp
    ${ENGINE_ENTITIES_DIR}/gate.cpp
    ${ENGINE_ENTITIES_DIR}/hitbox.cpp
    ${ENGINE_ENTITIES_DIR}/image.cpp
    ${ENGINE_ENTITIES_DIR}/item.cpp
    ${ENGINE_ENTITIES_DIR}/obstacle.cpp
    ${ENGINE_ENTITIES_DIR}/platform.cpp
    ${ENGINE_ENTITIES_DIR}/player.cpp
    ${ENGINE_ENTITIES_DIR}/png.cpp
    ${ENGINE_ENTITIES_DIR}/powerup.cpp
    ${ENGINE_ENTITIES_DIR}/projectile.cpp
    ${ENGINE_ENTITIES_DIR}/random_element.cpp
    ${ENGINE_ENTITIES_DIR}/sound.cpp
    ${ENGINE_ENTITIES_DIR}/spawner.cpp
    ${ENGINE_ENTITIES_DIR}/text.cpp
    ${ENGINE_ENTITIES_DIR}/trap.cpp
    ${ENGINE_ENTITIES_DIR}/triggerzone.cpp
    ${ENGINE_ENTITIES_DIR}/weapon.cpp
    ${ENGINE_UTILS_DIR}/entity_parser.cpp
    ${ENGINE_UTILS_DIR}/entity_storage.cpp
    ${ENGINE_UTILS_DIR}/serializer.cpp
    ${ENGINE_RENDERING_DIR}/Raylib.cpp
    ${ENGINE_RENDERING_DIR}/RenderUtils.cpp
    ${ENGINE_SCENE_DIR}/AScene.cpp
    ${ENGINE_SCENE_DIR}/ASceneHandler.cpp
    ${ENGINE_PHYSICS_DIR}/Collision.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CLIENT_DIR}
    ${CLIENT_DIR}/Handler
    ${ENGINE_CORE_DIR}/Include
    ${ENGINE_ENTITIES_DIR}/Include
    ${ENGINE_RENDERING_DIR}
    ${ENGINE_SCENE_DIR}/Include
    ${ENGINE_UTILS_DIR}/Include
    ${ENGINE_PHYSICS_DIR}/Include
    ${SHARED_DIR}
    ${SHARED_DIR}/Sockets/Include
)

target_link_libraries(r-type_client PRIVATE
    ecs
    game_logic
    shared_sockets
    raylib
    glfw
    nlohmann_json::nlohmann_json
    asio::asio
)

if(WIN32)
    target_compile_definitions(r-type_client PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER)
endif()

# -------------------------
# r-type_server executable
# -------------------------
add_executable(r-type_server
    ${SERVER_DIR}/main.cpp
    ${SERVER_DIR}/server.cpp
    ${SERVER_DIR}/connexion.cpp
    ${SERVER_DIR}/game_setup.cpp
    ${ENGINE_UTILS_DIR}/entity_parser.cpp
    ${ENGINE_UTILS_DIR}/entity_storage.cpp
    ${ENGINE_UTILS_DIR}/serializer.cpp
    ${ENGINE_ENTITIES_DIR}/background.cpp
    ${ENGINE_ENTITIES_DIR}/button.cpp
    ${ENGINE_ENTITIES_DIR}/checkpoint.cpp
    ${ENGINE_ENTITIES_DIR}/decoration.cpp
    ${ENGINE_ENTITIES_DIR}/enemy.cpp
    ${ENGINE_ENTITIES_DIR}/gate.cpp
    ${ENGINE_ENTITIES_DIR}/hitbox.cpp
    ${ENGINE_ENTITIES_DIR}/image.cpp
    ${ENGINE_ENTITIES_DIR}/item.cpp
    ${ENGINE_ENTITIES_DIR}/obstacle.cpp
    ${ENGINE_ENTITIES_DIR}/platform.cpp
    ${ENGINE_ENTITIES_DIR}/player.cpp
    ${ENGINE_ENTITIES_DIR}/png.cpp
    ${ENGINE_ENTITIES_DIR}/powerup.cpp
    ${ENGINE_ENTITIES_DIR}/projectile.cpp
    ${ENGINE_ENTITIES_DIR}/random_element.cpp
    ${ENGINE_ENTITIES_DIR}/sound.cpp
    ${ENGINE_ENTITIES_DIR}/spawner.cpp
    ${ENGINE_ENTITIES_DIR}/text.cpp
    ${ENGINE_ENTITIES_DIR}/trap.cpp
    ${ENGINE_ENTITIES_DIR}/triggerzone.cpp
    ${ENGINE_ENTITIES_DIR}/weapon.cpp
)

target_include_directories(r-type_server PRIVATE
    ${SERVER_DIR}/Include
    ${ENGINE_CORE_DIR}/Include
    ${ENGINE_ENTITIES_DIR}/Include
    ${ENGINE_UTILS_DIR}/Include
    ${SHARED_DIR}
    ${SHARED_DIR}/Sockets/Include
)

target_link_libraries(r-type_server PRIVATE
    ecs
    game_logic
    shared_sockets
    nlohmann_json::nlohmann_json
    asio::asio
)

if(WIN32)
    target_compile_definitions(r-type_server PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX NOGDI NOUSER)
endif()

# -------------------------
# Unit tests
# -------------------------
if(TU)
    message(STATUS "Building unit tests (TU=ON)")
    enable_testing()
    add_subdirectory(Unit_test)
else()
    message(STATUS "Skipping unit tests (use -DTU=ON to enable)")
endif()

# -------------------------
# Custom target to build with tests
# -------------------------
add_custom_target(tu
    COMMAND ${CMAKE_COMMAND} -DTU=ON -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
    COMMENT "Building project with unit tests enabled (-DTU=ON)"
)

# -------------------------
# Installation
# -------------------------
install(TARGETS r-type_client r-type_server ecs shared_sockets game_logic
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# -------------------------
# Assets setup
# -------------------------
# Copier les assets depuis Game/Assets vers le build directory
execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Game/Assets/
        ${CMAKE_BINARY_DIR}/assets
)

target_compile_definitions(r-type_client PUBLIC ASSETS_PATH="${CMAKE_BINARY_DIR}/assets")

# Définir également pour le serveur si nécessaire
target_compile_definitions(r-type_server PUBLIC ASSETS_PATH="${CMAKE_BINARY_DIR}/assets")