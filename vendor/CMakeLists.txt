# GLAD - OpenGL loader
add_library(glad STATIC
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c
)
target_include_directories(glad PUBLIC ${CMAKE_SOURCE_DIR}/vendor/glad/include)

# miniz - compression library
add_library(miniz STATIC
    ${CMAKE_SOURCE_DIR}/vendor/miniz/miniz.c
)
target_include_directories(miniz PUBLIC ${CMAKE_SOURCE_DIR}/vendor/miniz)
target_compile_definitions(miniz PRIVATE
    MINIZ_NO_ARCHIVE_APIS
    MINIZ_NO_ZLIB_APIS
    MINIZ_NO_STDIO
    MINIZ_NO_TIME
)
