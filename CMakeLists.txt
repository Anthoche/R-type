cmake_minimum_required(VERSION 3.16)

project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Directories
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Client)
set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Server)
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shared)
set(ENGINE_CORE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core)
set(GAME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Game)

# ECS library (Engine/Core)
add_library(ecs STATIC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
)

target_include_directories(ecs PUBLIC
    ${ENGINE_CORE_DIR}/Include
    ${SHARED_DIR}
)

# r-type_client (with SFML window)
find_package(SFML 2.5 REQUIRED COMPONENTS system window graphics)

add_executable(r-type_client
    ${CLIENT_DIR}/main.cpp
    ${CLIENT_DIR}/Network_client/client.cpp
    ${GAME_DIR}/Entities/player.cpp
    ${GAME_DIR}/Scene/game_scene.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CLIENT_DIR}/Network_client/Include
    ${CLIENT_DIR}/Input
    ${GAME_DIR}
    ${GAME_DIR}/Entities/Include
    ${GAME_DIR}/Scene/Include
    ${SHARED_DIR}
)

target_link_libraries(r-type_client PRIVATE ecs sfml-system sfml-window sfml-graphics)

# r-type_server (no SFML)
add_executable(r-type_server
    ${SERVER_DIR}/main.cpp
    ${SERVER_DIR}/Network_handler/server.cpp
    ${SERVER_DIR}/Network_handler/UDP_socket.cpp
)

target_include_directories(r-type_server PRIVATE
    ${SERVER_DIR}/Network_handler/Include
    ${SERVER_DIR}/Game_logic
    ${SHARED_DIR}
)

target_link_libraries(r-type_server PRIVATE ecs)

# Tests (later)
enable_testing()

# Installation
install(TARGETS r-type_client r-type_server ecs
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
