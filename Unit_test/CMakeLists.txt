cmake_minimum_required(VERSION 3.5)

# -------------------------
# Project setup
# -------------------------
set(OUTPUT UnitTests)
project(${OUTPUT} LANGUAGES CXX)

# -------------------------
# Source files to be tested
# -------------------------
set(SRC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
)

# -------------------------
# Header files
# -------------------------
set(HEAD
    ${ENGINE_CORE_DIR}/Include/entity.hpp
    ${ENGINE_CORE_DIR}/Include/registry.hpp
)

# -------------------------
# Unit test source files
# -------------------------
set(TESTS
    test_entity.cpp
    test_registry.cpp
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
    ${SHARED_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# -------------------------
# Link with Criterion
# -------------------------
target_link_libraries(${OUTPUT}
    criterion
)

# -------------------------
# Enable CTest integration
# -------------------------
enable_testing()
add_test(NAME ${OUTPUT} COMMAND ${OUTPUT})
