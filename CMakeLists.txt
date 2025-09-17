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


# Dependencies
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Directories
set(CLIENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Client)
set(ENGINE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine)
set(GAME_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Game)
set(SERVER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Server)
set(SHARED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shared)

# r-type_client
add_executable(r-type_client
    ${CLIENT_DIR}/main.cpp
    ${CLIENT_DIR}/Network_client/client.cpp
    ${ENGINE_DIR}/Rendering/Raylib.cpp
    ${ENGINE_DIR}/Rendering/window/AWindow.cpp
    ${ENGINE_DIR}/Rendering/window/WindowHandler.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CLIENT_DIR}/Network_client/Include
    ${CLIENT_DIR}/Input
    ${SHARED_DIR}
)

# r-type_server
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

# Linking
target_link_libraries(r-type_client PRIVATE raylib glfw)

# Tests (later)
enable_testing()

# Installation
install(TARGETS r-type_client r-type_server
    RUNTIME DESTINATION bin
)
