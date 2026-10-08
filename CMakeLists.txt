cmake_minimum_required(VERSION 3.16)

project(OpenGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)
list(FILTER SOURCES EXCLUDE REGEX ".*checkmate/.*")
list(FILTER SOURCES EXCLUDE REGEX ".*/build/.*")

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

find_program(GLSLC glslc REQUIRED)

set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
set(SHADER_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/shaders)

file(GLOB SHADER_SOURCES CONFIGURE_DEPENDS
    "${SHADER_DIR}/*.vert"
    "${SHADER_DIR}/*.frag"
)

set(SHADER_OUTPUTS)

foreach(SHADER ${SHADER_SOURCES})

    get_filename_component(SHADER_NAME ${SHADER} NAME)

    set(SPIRV
        "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv"
    )

    add_custom_command(
        OUTPUT "${SPIRV}"

        COMMAND ${CMAKE_COMMAND} -E make_directory
            "${SHADER_OUTPUT_DIR}"

        COMMAND ${GLSLC}
            "${SHADER}"
            -o "${SPIRV}"

        DEPENDS "${SHADER}"

        COMMENT "Compiling ${SHADER_NAME} -> ${SHADER_NAME}.spv"

        VERBATIM
    )

    list(APPEND SHADER_OUTPUTS "${SPIRV}")

endforeach()

add_custom_target(shaders ALL
    DEPENDS ${SHADER_OUTPUTS}
)

add_dependencies(OpenGame shaders)

