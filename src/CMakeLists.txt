# Create the static library for the engine
add_library(exodus_engine STATIC
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/camera_follow.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/chunk_generation.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/input.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/movement.cpp
        ${CMAKE_SOURCE_DIR}/src/ecs/systems/sprite_render.cpp
        ${CMAKE_SOURCE_DIR}/src/generation/generator.cpp
        ${CMAKE_SOURCE_DIR}/src/generation/noise.cpp
        ${CMAKE_SOURCE_DIR}/src/asset_manager.cpp
        ${CMAKE_SOURCE_DIR}/src/application.cpp
        ${CMAKE_SOURCE_DIR}/src/engine.cpp
        ${CMAKE_SOURCE_DIR}/src/factories.cpp
        ${CMAKE_SOURCE_DIR}/src/input.cpp
)
target_include_directories(exodus_engine PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)
target_compile_definitions(exodus_engine PRIVATE
        EXODUS_ASSETS_DIR="${CMAKE_SOURCE_DIR}/assets"
)
target_link_libraries(exodus_engine PUBLIC
        # TODO: Linking against these is causing problems when running the tests
        SDL2::SDL2
        SDL2_image::SDL2_image
        glad
)

# Link the OpenGL library based on the platform
if (WIN32)
    target_link_libraries(exodus_engine PUBLIC opengl32)
elseif (APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(exodus_engine PUBLIC ${OpenGL_LIBRARY})
else ()
    target_link_libraries(exodus_engine PUBLIC GL)
endif ()

# Create the exodus executable and link it with the required libraries
add_executable(exodus
        ${CMAKE_SOURCE_DIR}/src/main.cpp
)
target_link_libraries(exodus PRIVATE
        exodus_engine
)
