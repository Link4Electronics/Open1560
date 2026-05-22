cmake_minimum_required(VERSION 3.20)
project(Open1560 VERSION 1.0.0 LANGUAGES C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find required packages
find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype QUIET)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wno-missing-field-initializers -Wno-unused-private-field
        -Wno-overloaded-virtual -Wno-switch -Wno-invalid-offsetof
        -Wno-unused-parameter -Wno-ignored-qualifiers
        -Wno-missing-braces -Wno-enum-compare
        -fms-extensions -fno-exceptions -fno-rtti
        -fomit-frame-pointer -ffast-math)
endif()

add_compile_definitions(
    ARTS_DEV_BUILD=1
    ARTS_TARGET_BUILD=1560
    ARTS_STANDALONE
    _DEBUG
)

include_directories(
    ${CMAKE_SOURCE_DIR}/vendor/mem
    ${CMAKE_SOURCE_DIR}/vendor/hedley
    ${CMAKE_SOURCE_DIR}/vendor/miniz
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
    ${CMAKE_SOURCE_DIR}/code/midtown
    ${CMAKE_SOURCE_DIR}/game
    ${SDL3_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

# Third-party libraries
add_subdirectory(vendor)

# Game code
add_subdirectory(code)
