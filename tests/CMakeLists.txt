# Create the test executable
add_executable(exodus_core_tests
        ${CMAKE_SOURCE_DIR}/tests/ecs/systems/test_camera_follow.cpp
        ${CMAKE_SOURCE_DIR}/tests/ecs/systems/test_chunk_generation.cpp
        ${CMAKE_SOURCE_DIR}/tests/ecs/systems/test_collision_resolution.cpp
        ${CMAKE_SOURCE_DIR}/tests/ecs/systems/test_enemy.cpp
        ${CMAKE_SOURCE_DIR}/tests/ecs/systems/test_input.cpp
        ${CMAKE_SOURCE_DIR}/tests/ecs/systems/test_movement.cpp
        ${CMAKE_SOURCE_DIR}/tests/ecs/systems/test_sprite_render.cpp
        ${CMAKE_SOURCE_DIR}/tests/ecs/test_registry.cpp
        ${CMAKE_SOURCE_DIR}/tests/generation/test_generator.cpp
        ${CMAKE_SOURCE_DIR}/tests/generation/test_noise.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_camera.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_engine.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_factories.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_math.cpp
        ${CMAKE_SOURCE_DIR}/tests/test_steering.cpp
)
target_include_directories(exodus_core_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/tests
)
target_link_libraries(exodus_core_tests PRIVATE
        exodus_core
        GTest::gtest_main
)

# Add the test to CTest
add_test(NAME Tests COMMAND exodus_core_tests)
