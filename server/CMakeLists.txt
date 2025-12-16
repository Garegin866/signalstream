cmake_minimum_required(VERSION 3.20)
project(signalstream_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)
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
)

set(SERVICES
        services/TagsService.cpp
        services/AuthService.cpp
        services/UserTagService.cpp
        services/FeedService.cpp
        services/ItemsService.cpp
        services/ItemTagService.cpp
        services/AdminService.cpp
        services/RoleService.cpp
        services/NotificationService.cpp
)

set(CONTROLLERS
        controllers/TagsController.cpp
        controllers/AuthController.cpp
        controllers/UserTagController.cpp
        controllers/FeedController.cpp
        controllers/ItemTagsController.cpp
        controllers/ItemsController.cpp
        controllers/AdminController.cpp
)

set(MIDDLEWARE
        middleware/TokenMiddleware.cpp
        middleware/UserContextMiddleware.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
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
target_link_libraries(${PROJECT_NAME} PRIVATE argon2::argon2)