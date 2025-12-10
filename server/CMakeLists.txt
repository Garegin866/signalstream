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
)

set(SERVICES
        services/TagsService.cpp
        services/AuthService.cpp
)

set(CONTROLLERS
        controllers/TagsController.cpp
        controllers/AuthController.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        ${REPOSITORIES}
        ${SERVICES}
        ${CONTROLLERS}
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