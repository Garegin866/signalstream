cmake_minimum_required(VERSION 3.20)
project(signalstream_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)
find_package(CURL REQUIRED)
find_package(argon2 REQUIRED CONFIG)

set(REPOSITORIES
        repositories/TagsRepository.cpp
        repositories/SessionRepository.cpp
        repositories/UserRepository.cpp
        repositories/UserTagsRepository.cpp
        repositories/ItemsRepository.cpp
        repositories/ItemTagsRepository.cpp
        repositories/FeedRepository.cpp
        repositories/NotificationRepository.cpp
        repositories/ResetTokenRepository.cpp
)

set(SERVICES
        services/TagsService.cpp
        services/AuthService.cpp
        services/UserTagService.cpp
        services/FeedService.cpp
        services/ItemsService.cpp
        services/AdminService.cpp
        services/RoleService.cpp
        services/NotificationService.cpp
        services/EmailService.cpp
)

set(CONTROLLERS
        controllers/TagsController.cpp
        controllers/AuthController.cpp
        controllers/UserTagController.cpp
        controllers/FeedController.cpp
        controllers/ItemsController.cpp
        controllers/AdminController.cpp
        controllers/NotificationController.cpp
        controllers/DocsController.cpp
)

set(MIDDLEWARE
        middleware/TokenMiddleware.cpp
        middleware/UserContextMiddleware.cpp
        middleware/RequestLoggingMiddleware.cpp
)

set(CORE
        core/ErrorLogger.cpp
)

set(UTILS
        utils/SmtpClient.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${CORE}
        ${UTILS}
        ${REPOSITORIES}
        ${SERVICES}
        ${CONTROLLERS}
        ${MIDDLEWARE}
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/controllers
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/dto
        ${CMAKE_CURRENT_SOURCE_DIR}/repositories
        ${CMAKE_CURRENT_SOURCE_DIR}/services
)
#
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)
target_link_libraries(${PROJECT_NAME} PRIVATE argon2::argon2)