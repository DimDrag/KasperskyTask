# создание динамической библиотеки scanner_core
add_library(scanner_core SHARED
    scanner/Scanner.h
    scanner/Scanner.cpp
    scanner/Hash.h
    scanner/MalwareHashBase.h
    scanner/MalwareHashBase.cpp
    scanner/Logger.h
    scanner/Logger.cpp
    scanner/MD5FileHasher.h
    scanner/MD5FileHasher.cpp
    utils/Clock.h
    ../external/hash-library/md5.h
    ../external/hash-library/md5.cpp
)

target_include_directories(scanner_core PRIVATE ${CMAKE_SOURCE_DIR}/external)
target_include_directories(scanner_core PRIVATE ${CMAKE_SOURCE_DIR}/src)

# создание исполняемого файла scanner
add_executable(scanner
    main.cpp
)

target_link_libraries(scanner PRIVATE scanner_core)

target_include_directories(scanner PRIVATE ${CMAKE_SOURCE_DIR}/external/cxxopts/include)
