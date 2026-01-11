# Add GLAD for OpenGL function loading
add_library(glad STATIC ${CMAKE_SOURCE_DIR}/external/glad/src/gl.c)
target_include_directories(glad SYSTEM PUBLIC ${CMAKE_SOURCE_DIR}/external/glad/include)
