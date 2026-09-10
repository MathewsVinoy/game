cmake_minimum_required(VERSION 3.20)

project(game
    VERSION 0.1.0
    LANGUAGES CXX
)

file(GLOB_RECURSE GAME_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)


file(GLOB_RECURSE GAME_HEADERS
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
)


add_executable(game ${GAME_SOURCES} ${GAME_HEADERS})


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


target_include_directories(game
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(game
    PRIVATE
        engine
)