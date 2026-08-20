cmake_minimum_required(VERSION 3.16)

project(engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

add_executable(engine
    src/main.cpp
)

target_link_libraries(engine
    PRIVATE
    glfw
    Vulkan::Vulkan
    glm::glm
)