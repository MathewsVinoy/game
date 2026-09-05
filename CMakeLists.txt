cmake_minimum_required(VERSION 3.20)

project(OpenGame
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_EXTENSIONS OFF)

execute_process(
    COMMAND /bin/sh ${CMAKE_SOURCE_DIR}/tests/compile.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests
    RESULT_VARIABLE SCRIPT_RESULT
)

if(NOT SCRIPT_RESULT EQUAL 0)
    message(WARNING
        "tests/compile.sh returned ${SCRIPT_RESULT}"
    )
endif()

add_subdirectory(engine)