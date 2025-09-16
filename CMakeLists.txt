cmake_minimum_required(VERSION 3.16)

project(rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Configuration des répertoires
set(ENGINE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine/Core/Include)
set(ENGINE_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Engine)
set(GAME_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Game)

# Bibliothèque ECS (Engine Core)
add_library(ecs STATIC
	${ENGINE_SRC_DIR}/Core/entity.cpp
	${ENGINE_SRC_DIR}/Core/registry.cpp
)

target_include_directories(ecs PUBLIC ${ENGINE_INCLUDE_DIR} ${GAME_INCLUDE_DIR})

# Exécutable principal de démonstration
add_executable(rtype_demo ${ENGINE_SRC_DIR}/Core/main.cpp)
target_link_libraries(rtype_demo PRIVATE ecs)
target_include_directories(rtype_demo PRIVATE ${ENGINE_INCLUDE_DIR})

# Jeu principal R-Type
add_executable(rtype_game 
	${GAME_INCLUDE_DIR}/main.cpp 
	${ENGINE_SRC_DIR}/Core/entity.cpp 
	${ENGINE_SRC_DIR}/Core/registry.cpp
	${GAME_INCLUDE_DIR}/Entities/player.cpp
	${GAME_INCLUDE_DIR}/Scene/game_scene.cpp
)
target_link_libraries(rtype_game PRIVATE ecs)
target_include_directories(rtype_game PRIVATE ${ENGINE_INCLUDE_DIR} ${GAME_INCLUDE_DIR})

# Configuration pour les tests unitaires
enable_testing()

# Installation
install(TARGETS ecs rtype_demo rtype_game
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)
