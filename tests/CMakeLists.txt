add_executable(test_scanner
    test_main.cpp
    tests.h
    base_tests.cpp
    logger_tests.cpp
    scanner_tests.cpp
    helpers/TemporaryPath.h
    helpers/TemporaryPath.cpp
    helpers/TemporaryFile.h
    helpers/TemporaryFile.cpp
    helpers/TemporaryDirectory.h
    helpers/TemporaryDirectory.cpp
)

target_include_directories(test_scanner PRIVATE ${CMAKE_SOURCE_DIR}/tests/helpers)
target_include_directories(test_scanner PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(test_scanner PRIVATE ${CMAKE_SOURCE_DIR}/src/utils)

target_link_libraries(test_scanner PRIVATE
    scanner_core
    gtest
    gtest_main
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

target_compile_definitions(test_scanner PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_BINARY_DIR}/data"
)

include(GoogleTest)
gtest_discover_tests(test_scanner)
