set(TEST_SCANNER_HEADERS
    tests.h
    helpers/TemporaryPath.h
    helpers/TemporaryFile.h
    helpers/TemporaryDirectory.h
)

set(TEST_SCANNER_SOURCES
    test_main.cpp
    base_tests.cpp
    logger_tests.cpp
    scanner_tests.cpp
    helpers/TemporaryPath.cpp
    helpers/TemporaryFile.cpp
    helpers/TemporaryDirectory.cpp
)

add_executable(test_scanner
    ${TEST_SCANNER_HEADERS}
    ${TEST_SCANNER_SOURCES}
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

# чтобы тесты всегда могли найти их тренировочные файлы, независимо откуда их запускают
target_compile_definitions(test_scanner PRIVATE
    TEST_DATA_DIR="${CMAKE_CURRENT_BINARY_DIR}/data"
)

include(GoogleTest)
gtest_discover_tests(test_scanner)
