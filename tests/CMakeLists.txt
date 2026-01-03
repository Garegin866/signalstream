find_package(Catch2 REQUIRED)

add_executable(test_signalstream
        main.cpp
        unit/dummy_test.cpp
)

target_link_libraries(test_signalstream PRIVATE Catch2::Catch2WithMain)
target_compile_features(test_signalstream PRIVATE cxx_std_20)

add_test(NAME test_signalstream COMMAND test_signalstream)