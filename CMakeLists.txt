cmake_minimum_required(VERSION 3.10)
project(DSP_Project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wpedantic)
file(GLOB SOURCES "src/*.cpp")
include_directories(include)

add_executable(DSP_App ${SOURCES})

add_custom_target(copy_test_data ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/test_data
    ${CMAKE_BINARY_DIR}/test_data
    COMMENT "Kopiowanie test_data do katalogu build"
)

add_dependencies(DSP_App copy_test_data)