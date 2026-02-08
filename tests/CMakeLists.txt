# Create the test executable
add_executable(exodus_core_tests
        ${CMAKE_SOURCE_DIR}/tests/ecs/test_registry.cpp
        ${CMAKE_SOURCE_DIR}/tests/generation/test_noise.cpp
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
