cmake_minimum_required(VERSION 3.20)

project(engine
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(engine STATIC
    ${ENGINE_SOURCES}
)

target_compile_features(engine
    PRIVATE
        cxx_std_20
)

target_include_directories(engine
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"

    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyobjloader"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/tinygltf"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FastNoise"
)

target_link_libraries(engine
    PRIVATE
        glfw
        Vulkan::Vulkan
        glm::glm
        assimp::assimp 
)

if(MSVC)

    target_compile_options(engine
        PRIVATE
            /W4
    )

else()

    target_compile_options(engine
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-sign-compare
            -Wno-unused-parameter
    )

endif()

set(STB_IMAGE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/lib"
)

set(STB_IMAGE_HEADER
    "${STB_IMAGE_DIR}/stb_image.h"
)

file(MAKE_DIRECTORY
    "${STB_IMAGE_DIR}"
)

if(NOT EXISTS "${STB_IMAGE_HEADER}")

    message(STATUS
        "stb_image.h not found. Downloading..."
    )

    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
        "${STB_IMAGE_HEADER}"
        SHOW_PROGRESS
        STATUS STB_DOWNLOAD_STATUS
    )

    list(GET STB_DOWNLOAD_STATUS 0 STB_DOWNLOAD_ERROR)

    if(NOT STB_DOWNLOAD_ERROR EQUAL 0)

        file(REMOVE
            "${STB_IMAGE_HEADER}"
        )

        message(FATAL_ERROR
            "Failed to download stb_image.h: ${STB_DOWNLOAD_STATUS}"
        )

    endif()

    message(STATUS
        "Downloaded stb_image.h to ${STB_IMAGE_HEADER}"
    )

else()

    message(STATUS
        "Found stb_image.h: ${STB_IMAGE_HEADER}"
    )

endif()


find_program(GLSL_VALIDATOR
    NAMES glslangValidator

    HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        "$ENV{VULKAN_SDK}/bin"
        "$ENV{VULKAN_SDK}/Bin"

    PATHS
        /usr/bin
        /usr/local/bin
)

if(NOT GLSL_VALIDATOR)

    message(FATAL_ERROR
        "glslangValidator not found. "
        "Please install the Vulkan SDK or add "
        "glslangValidator to your PATH."
    )

endif()

message(STATUS
    "Found glslangValidator: ${GLSL_VALIDATOR}"
)

file(GLOB_RECURSE GLSL_SOURCE_FILES
    CONFIGURE_DEPENDS

    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag"
)

set(SPIRV_BINARY_FILES)

foreach(GLSL ${GLSL_SOURCE_FILES})

    get_filename_component(FILE_NAME
        "${GLSL}"
        NAME
    )

    set(SPIRV
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${FILE_NAME}.spv"
    )

    add_custom_command(
        OUTPUT
            "${SPIRV}"

        COMMAND
            "${GLSL_VALIDATOR}"
            -V
            "${GLSL}"
            -o
            "${SPIRV}"

        DEPENDS
            "${GLSL}"

        COMMENT
            "Compiling shader ${FILE_NAME}"

        VERBATIM
    )

    list(APPEND
        SPIRV_BINARY_FILES
        "${SPIRV}"
    )

endforeach()


add_custom_target(Shaders
    DEPENDS
        ${SPIRV_BINARY_FILES}
)

add_dependencies(engine
    Shaders
)


set_property(
    TARGET engine
    PROPERTY VS_DEBUGGER_WORKING_DIRECTORY
    "${CMAKE_SOURCE_DIR}"
)