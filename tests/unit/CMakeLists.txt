project(signalstream_unit_tests LANGUAGES CXX)

# ----------------------------------
# Dependencies
# ----------------------------------
find_package(Catch2 3 REQUIRED)

# ----------------------------------
# Test executable
# ----------------------------------
set(UNIT_TESTS
        test_token_generator.cpp
        test_password_hasher.cpp
        test_error.cpp
        test_response_helpers.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${UNIT_TESTS}
)

# ----------------------------------
# Include paths
# ----------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/server
)

# ----------------------------------
# Link libraries
# ----------------------------------
target_link_libraries(${PROJECT_NAME} PRIVATE
        signalstream_core
        Catch2::Catch2WithMain
)

# ----------------------------------
# Register tests with CTest
# ----------------------------------
add_test(
        NAME unit_tests
        COMMAND ${PROJECT_NAME}
)