cmake_minimum_required(VERSION 3.16)

project(OpenGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    main/*.cpp
    main/*.hpp
    core/*.cpp
    core/*.hpp
    graphics/*.hpp
    graphics/*.cpp
    inputs/*.hpp
    inputs/*.h
    inputs/*.cpp
    player/*.cpp
    player/*.hpp
)

add_executable(OpenGame ${SOURCES})

target_include_directories(OpenGame PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tinyobjloader
)

target_compile_options(OpenGame PRIVATE
    -O2
)

target_link_libraries(OpenGame PRIVATE
    glfw
    vulkan
    dl
    pthread
    X11
    Xxf86vm
    Xrandr
    Xi
    assimp
)
