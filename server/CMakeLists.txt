cmake_minimum_required(VERSION 3.20)
project(signalstream_server LANGUAGES CXX)

# ----------------------------------
# Server executable
# ----------------------------------
set(CONTROLLERS
        controllers/TagsController.cpp
        controllers/AuthController.cpp
        controllers/UserTagController.cpp
        controllers/FeedController.cpp
        controllers/ItemsController.cpp
        controllers/AdminController.cpp
        controllers/NotificationController.cpp
        controllers/DocsController.cpp
        controllers/HealthController.cpp
)

set(MIDDLEWARE
        middleware/TokenMiddleware.cpp
        middleware/UserContextMiddleware.cpp
        middleware/RequestLoggingMiddleware.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${CONTROLLERS}
        ${MIDDLEWARE}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        signalstream_core
        Drogon::Drogon
        CURL::libcurl
        argon2::argon2
)

# ----------------------------------
# Warnings
# ----------------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(signalstream_core PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()