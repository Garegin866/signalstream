cmake_minimum_required(VERSION 3.20)
project(signalstream LANGUAGES CXX)

# ----------------------------------
# Global settings
# ----------------------------------
option(ENABLE_COVERAGE "Enable code coverage (gcov)" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (ENABLE_COVERAGE)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR "Coverage is only supported with GCC or Clang")
    endif()
endif()

# ----------------------------------
# Coverage configuration (ADDED)
# ----------------------------------
if (ENABLE_COVERAGE)
    add_library(coverage_config INTERFACE)

    target_compile_options(coverage_config INTERFACE
            --coverage
            -O0
            -g
    )

    target_link_options(coverage_config INTERFACE
            --coverage
    )

    # Ensure no optimized build sneaks in
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

# ----------------------------------
# Dependencies
# ----------------------------------
find_package(Drogon REQUIRED)
find_package(CURL REQUIRED)
find_package(argon2 REQUIRED CONFIG)

# ----------------------------------
# Core application library
# ----------------------------------
set(REPOSITORIES
        server/repositories/TagsRepository.cpp
        server/repositories/SessionRepository.cpp
        server/repositories/UserRepository.cpp
        server/repositories/UserTagsRepository.cpp
        server/repositories/ItemsRepository.cpp
        server/repositories/ItemTagsRepository.cpp
        server/repositories/FeedRepository.cpp
        server/repositories/NotificationRepository.cpp
        server/repositories/ResetTokenRepository.cpp
)

set(SERVICES
        server/services/TagsService.cpp
        server/services/AuthService.cpp
        server/services/UserTagService.cpp
        server/services/FeedService.cpp
        server/services/ItemsService.cpp
        server/services/AdminService.cpp
        server/services/RoleService.cpp
        server/services/NotificationService.cpp
        server/services/EmailService.cpp
)

set(CORE
        server/core/ErrorLogger.cpp
)

set(UTILS
        server/utils/SmtpClient.cpp
)

add_library(signalstream_core
        ${CORE}
        ${UTILS}
        ${REPOSITORIES}
        ${SERVICES}
)

target_include_directories(signalstream_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/server
        ${CMAKE_CURRENT_SOURCE_DIR}/controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/dto
        ${CMAKE_CURRENT_SOURCE_DIR}/repositories
        ${CMAKE_CURRENT_SOURCE_DIR}/services
)

target_link_libraries(signalstream_core PUBLIC
        Drogon::Drogon
        CURL::libcurl
        argon2::argon2
)

# ----------------------------------
# Attach coverage ONLY when enabled (ADDED)
# ----------------------------------
if (ENABLE_COVERAGE)
    target_link_libraries(signalstream_core PRIVATE coverage_config)
endif()

# ----------------------------------
# Tests & subdirs
# ----------------------------------
enable_testing()

add_subdirectory(server)
add_subdirectory(tests)