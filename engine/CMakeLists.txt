cmake_minimum_required(VERSION 3.20)

project(engine
    VERSION 0.1.0
    LANGUAGES CXX
)


set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

file(GLOB_RECURSE ENGINE_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_executable(engine
    ${ENGINE_SOURCES}
)

target_compile_features(engine
    PRIVATE
        cxx_std_20
)

target_include_directories(engine
    PRIVATE

        "${CMAKE_CURRENT_SOURCE_DIR}/include"

        "${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyobjloader"
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/FastNoise"


)


target_link_libraries(engine
    PRIVATE
        glfw
        Vulkan::Vulkan
        glm::glm
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

find_program(GLSL_VALIDATOR
    glslangValidator

    HINTS
        ${Vulkan_GLSLANG_VALIDATOR_EXECUTABLE}
        /usr/bin
        /usr/local/bin
        $ENV{VULKAN_SDK}/bin
        $ENV{VULKAN_SDK}/Bin
)


if(NOT GLSL_VALIDATOR)

    message(FATAL_ERROR
        "glslangValidator not found."
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
        ${GLSL}
        NAME
    )

    set(SPIRV
        "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${FILE_NAME}.spv"
    )

    add_custom_command(
        OUTPUT
            ${SPIRV}

        COMMAND
            ${GLSL_VALIDATOR}
            -V
            ${GLSL}
            -o
            ${SPIRV}

        DEPENDS
            ${GLSL}

        COMMENT
            "Compiling shader ${FILE_NAME}"
    )

    list(APPEND
        SPIRV_BINARY_FILES
        ${SPIRV}
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
