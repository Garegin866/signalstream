project(signalstream_integration_tests LANGUAGES CXX)

# ----------------------------------
# Dependencies
# ----------------------------------
find_package(Catch2 3 REQUIRED)

# ----------------------------------
# Test executable
# ----------------------------------
set(INTEGRATION_TESTS
        repositories/test_user_repository.cpp
        repositories/test_items_repository.cpp
        repositories/test_tags_repository.cpp
        repositories/test_item_tags_repository.cpp
        repositories/test_user_tags_repository.cpp
        repositories/test_feed_repository.cpp

        services/test_auth_service.cpp
        services/test_tags_service.cpp
        services/test_items_service.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${INTEGRATION_TESTS}
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
        Catch2::Catch2
)

# ----------------------------------
# Register tests with CTest
# ----------------------------------
add_test(
        NAME integration_tests
        COMMAND ${PROJECT_NAME}
)