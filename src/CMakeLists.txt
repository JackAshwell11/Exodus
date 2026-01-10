# Add source files for the exodus executable
add_executable(exodus main.cpp)

# Link the required libraries
target_link_libraries(exodus PRIVATE
    SDL2::SDL2
    glad
)

# Link the OpenGL library based on the platform
if (WIN32)
    target_link_libraries(exodus PRIVATE opengl32)
elseif (APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(exodus PRIVATE ${OpenGL_LIBRARY})
else()
    target_link_libraries(exodus PRIVATE GL)
endif()
