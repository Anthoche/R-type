cmake_minimum_required(VERSION 3.5)


set(OUTPUT UnitTests)


project(${OUTPUT} LANGUAGES CXX)


set(SRC
    ${ENGINE_CORE_DIR}/entity.cpp
    ${ENGINE_CORE_DIR}/registry.cpp
)


set(HEAD
    ${ENGINE_CORE_DIR}/Include/entity.hpp
    ${ENGINE_CORE_DIR}/Include/registry.hpp
)


set(TESTS
    test_entity.cpp
    test_registry.cpp
)


add_executable(${OUTPUT} ${SRC} ${HEAD} ${TESTS})


target_include_directories(${OUTPUT} PRIVATE
    ${ENGINE_CORE_DIR}/Include
    ${SHARED_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)


target_link_libraries(${OUTPUT}
    criterion
)


enable_testing()
add_test(NAME ${OUTPUT} COMMAND ${OUTPUT})
