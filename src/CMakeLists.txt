# создание динамической библиотеки scanner_core
set(SCANNER_CORE_HEADERS
    scanner/Scanner.h
    scanner/Hash.h
    scanner/MalwareHashBase.h
    scanner/Logger.h
    scanner/MD5FileHasher.h
    utils/Clock.h
    ../external/hash-library/md5.h
)

set(SCANNER_CORE_SOURCES
    scanner/Scanner.cpp
    scanner/MalwareHashBase.cpp
    scanner/Logger.cpp
    scanner/MD5FileHasher.cpp
    ../external/hash-library/md5.cpp
)

add_library(scanner_core SHARED
    ${SCANNER_CORE_HEADERS}
    ${SCANNER_CORE_SOURCES}
)

target_include_directories(scanner_core PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_include_directories(scanner_core PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(scanner_core PRIVATE ${CMAKE_SOURCE_DIR}/src/utils)

# создание исполняемого файла scanner
add_executable(scanner
    main.cpp
)

target_link_libraries(scanner PRIVATE scanner_core)

target_include_directories(scanner PRIVATE ${CMAKE_SOURCE_DIR}/external/cxxopts/include)
target_include_directories(scanner PRIVATE ${CMAKE_SOURCE_DIR}/src/utils)
