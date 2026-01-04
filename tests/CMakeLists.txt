find_package(JsonCpp REQUIRED)
find_package(Catch2 3 REQUIRED)
find_package(argon2 REQUIRED)
find_package(Drogon REQUIRED)

add_executable(test_signalstream
        unit/test_token_generator.cpp
        unit/test_password_hasher.cpp
        unit/test_error.cpp
        unit/test_response_helpers.cpp
)

target_include_directories(test_signalstream PRIVATE
        ${CMAKE_SOURCE_DIR}/server
)

target_link_libraries(test_signalstream PRIVATE
        JsonCpp::JsonCpp
        Catch2::Catch2WithMain
        argon2::argon2
        Drogon::Drogon
)

add_test(
        NAME signalstream_tests
        COMMAND test_signalstream
)