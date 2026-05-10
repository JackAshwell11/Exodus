# Create the static library for the core functionality of the engine
add_library(exodus_core STATIC
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/camera_follow.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/chunk_generation.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/collision_resolution.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/enemy.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/input.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/movement.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/sprite_render.cpp
        ${CMAKE_SOURCE_DIR}/src/generation/generator.cpp
        ${CMAKE_SOURCE_DIR}/src/generation/noise.cpp
        ${CMAKE_SOURCE_DIR}/src/engine.cpp
        ${CMAKE_SOURCE_DIR}/src/factories.cpp
)
target_include_directories(exodus_core PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

# Create the exodus executable and link it with the required libraries
add_executable(exodus
        ${CMAKE_SOURCE_DIR}/src/rendering/opengl_renderer.cpp
        ${CMAKE_SOURCE_DIR}/src/application.cpp
        ${CMAKE_SOURCE_DIR}/src/main.cpp
)
target_include_directories(exodus PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)
target_compile_definitions(exodus PRIVATE
        EXODUS_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
)
target_link_libraries(exodus PRIVATE
        exodus_core
        SDL2::SDL2
        SDL2_image::SDL2_image
        OpenGL::GL
        glad
)
