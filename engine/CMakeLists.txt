cmake_minimum_required(VERSION 3.20)

project(engine
    LANGUAGES CXX
)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(engine ${ENGINE_SOURCES})

target_include_directories(engine
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(engine
    PRIVATE
        glfw
        Vulkan::Vulkan
        glm::glm
)

target_compile_features(engine
    PRIVATE
        cxx_std_20
)

if(MSVC)
    target_compile_options(engine PRIVATE
        /W4
    )
else()
    target_compile_options(engine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()