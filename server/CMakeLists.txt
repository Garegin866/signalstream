cmake_minimum_required(VERSION 3.20)
project(signalstream_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Drogon REQUIRED)

add_executable(signalstream
        main.cpp
        controllers/HealthController.cpp
)

target_include_directories(signalstream PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/controllers
)

target_link_libraries(signalstream PRIVATE Drogon::Drogon)
