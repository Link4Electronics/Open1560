# Collect all source files
file(GLOB_RECURSE ARTS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/agi/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/agigl/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/agirend/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/agisdl/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/agisw/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/agiworld/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/arts7/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/data7/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/dyna7/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/eventq7/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/localize/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/memory/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmai/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmanim/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmaudio/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmbangers/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmcamtour/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmcamcs/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmcar/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmcity/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmcityinfo/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmdjpeg/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmdyna/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmeffects/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmgame/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mminput/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmnetwork/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmphysics/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmui/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmvid/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mmwidget/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/pcwindis/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/stream/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/toolmgr/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vector7/*.cpp"
)

# Exclude problematic Windows-specific files
list(REMOVE_ITEM ARTS_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/code/midtown/midtown.rc"
)

# Game assembly - try assembling with yasm or provide stubs
# For now, game.asm is excluded and we'll provide stubs

# Include directories
set(ARTS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
    # core/ intentionally omitted to avoid shadowing system <endian.h>
    ${CMAKE_SOURCE_DIR}/vendor/mem
    ${CMAKE_SOURCE_DIR}/vendor/hedley
    ${CMAKE_SOURCE_DIR}/vendor/miniz
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
    ${SDL3_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/vendor/stubs
    ${FREETYPE_INCLUDE_DIRS}
)

include_directories(${ARTS_INCLUDE_DIRS})

# Precompiled header
set(ARTS_PCH "${CMAKE_CURRENT_SOURCE_DIR}/arts_pch.h")

# Build all source files as a single static library for now
add_library(arts_objs OBJECT ${ARTS_SOURCES})
target_include_directories(arts_objs PUBLIC ${ARTS_INCLUDE_DIRS})
target_compile_definitions(arts_objs PRIVATE
    ARTS_DEV_BUILD=1
    ARTS_TARGET_BUILD=1560
    ARTS_STANDALONE
    _DEBUG
    ARTS_ENABLE_KNI
    HEDLEY_STATIC_ASSERT=1
)

target_compile_options(arts_objs PRIVATE
    -include "${CMAKE_CURRENT_SOURCE_DIR}/arts_pch.h"
    -Wno-missing-field-initializers
    -Wno-unused-private-field
    -Wno-overloaded-virtual
    -Wno-switch
    -Wno-invalid-offsetof
    -Wno-unused-parameter
    -Wno-ignored-qualifiers
    -Wno-missing-braces
    -Wno-enum-compare
    -fms-extensions
    -fno-exceptions
    -fno-rtti
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(arts_objs PRIVATE
        -Wno-undefined-reinterpret-cast
        -Wno-deprecated-volatile
        -Wno-deprecated-declarations
        -Wno-nonportable-system-include-path
    )
endif()

# Main executable
add_executable(Open1560
    ${CMAKE_CURRENT_SOURCE_DIR}/midtown.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/midtown.h
    ${CMAKE_CURRENT_SOURCE_DIR}/game_stubs.S
)

target_include_directories(Open1560 PRIVATE ${ARTS_INCLUDE_DIRS})
target_compile_definitions(Open1560 PRIVATE
    ARTS_DEV_BUILD=1
    ARTS_TARGET_BUILD=1560
    ARTS_STANDALONE
    _DEBUG
)

# Only apply PCH and C++ flags to C++ sources (not assembly)
target_compile_options(Open1560 PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-include "${ARTS_PCH}">
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-missing-field-initializers>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-private-field>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-overloaded-virtual>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-switch>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-invalid-offsetof>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-unused-parameter>
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-ignored-qualifiers>
    $<$<COMPILE_LANGUAGE:CXX>:-fms-extensions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(JPEG libjpeg)
    pkg_check_modules(AVFORMAT libavformat)
    pkg_check_modules(AVCODEC libavcodec)
    pkg_check_modules(SWSCALE libswscale)
    pkg_check_modules(AVUTIL libavutil)
endif()

if(AVFORMAT_FOUND AND AVCODEC_FOUND AND SWSCALE_FOUND AND AVUTIL_FOUND)
    target_compile_definitions(arts_objs PRIVATE ARTS_HAVE_FFMPEG)
    target_compile_definitions(Open1560 PRIVATE ARTS_HAVE_FFMPEG)
endif()

target_link_libraries(Open1560
    arts_objs
    glad
    miniz
    SDL3::SDL3
    OpenGL::GL
    ${FREETYPE_LIBRARIES}
    ${JPEG_LIBRARIES}
    ${AVFORMAT_LIBRARIES}
    ${AVCODEC_LIBRARIES}
    ${SWSCALE_LIBRARIES}
    ${AVUTIL_LIBRARIES}
    dl
    pthread
    m
)

target_include_directories(Open1560 PRIVATE ${JPEG_INCLUDE_DIRS} ${AVFORMAT_INCLUDE_DIRS} ${AVCODEC_INCLUDE_DIRS} ${SWSCALE_INCLUDE_DIRS} ${AVUTIL_INCLUDE_DIRS})

# Copy binary to game directory after build
add_custom_command(TARGET Open1560 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Open1560>
        ${CMAKE_SOURCE_DIR}/game/Open1560
    COMMENT "Copying Open1560 binary to game directory"
)

target_include_directories(Open1560 PRIVATE ${JPEG_INCLUDE_DIRS})
