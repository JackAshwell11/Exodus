# Create the test executable
add_executable(exodus_tests
        ${CMAKE_SOURCE_DIR}/tests/ecs/test_registry.cpp
)
target_include_directories(exodus_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(exodus_tests
        PRIVATE exodus_engine
        PRIVATE GTest::gtest_main
)

# Add the test to CTest
add_test(NAME Tests COMMAND exodus_tests)
