# Define some CMake options
cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan_provider.cmake)

# Define custom options
set(ENABLE_COVERAGE ON CACHE BOOL "Enable coverage reporting for GCC/Clang")
set(ENABLE_SANITIZERS ON CACHE BOOL "Enable sanitizers for GCC/Clang")
set(DO_TESTS ON CACHE BOOL "Enable testing")

# Initialise the project
project(exodus LANGUAGES C CXX)

# Include the required modules
include(CTest)

# Find the required packages
find_package(SDL2 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Set the warning level for the project
if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif ()

# Enable coverage if supported by the compiler and enabled by the user
if (ENABLE_COVERAGE)
    if (NOT MSVC)
        message(STATUS "Coverage enabled (compiler is GCC/Clang)")
        add_compile_options(-fprofile-arcs -ftest-coverage)
        add_link_options(-fprofile-arcs -ftest-coverage)
    else ()
        message(STATUS "Coverage disabled (compiler is not GCC/Clang)")
    endif ()
else ()
    message(STATUS "Coverage disabled (ENABLE_COVERAGE is not set)")
endif ()

# Enable sanitizers if supported by the compiler and enabled by the user
if (ENABLE_SANITIZERS)
    if (NOT MSVC)
        message(STATUS "Sanitizers enabled (compiler is GCC/Clang)")
        add_compile_options(-fsanitize=address,undefined)
        add_link_options(-fsanitize=address,undefined)
    else ()
        message(STATUS "Sanitizers disabled (compiler is not GCC/Clang)")
    endif ()
else ()
    message(STATUS "Sanitizers disabled (ENABLE_SANITIZERS is not set)")
endif ()

# Add external libraries
add_subdirectory(external)

# Add the subdirectories for the different parts of the project
add_subdirectory(src)
if (DO_TESTS)
    message(STATUS "Tests enabled (DO_TESTS is set)")
    add_subdirectory(tests)
else ()
    message(STATUS "Tests disabled (DO_TESTS is not set)")
endif ()
