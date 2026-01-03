cmake_minimum_required(VERSION 3.20)
project(signalstream LANGUAGES CXX)

enable_testing()

add_subdirectory(server)
add_subdirectory(tests)