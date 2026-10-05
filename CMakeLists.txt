cmake_minimum_required(VERSION 3.16)

project(OpenGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    main/*.cpp
    main/*.hpp
    main/*.h
)

add_executable(OpenGame ${SOURCES})

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
)
