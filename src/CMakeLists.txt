cmake_minimum_required(VERSION 3.15) # минимальная версия CMake
project(scanner LANGUAGES CXX)

# Запрещаем сборку прямо в src/
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
        "Сборка внутри каталога src запрещена.\n"
        "Создайте каталог ../build и соберите проект так:\n"
        "  cmake -S src -B build")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Автоматический экспорт всех символов в DLL
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Каталоги вывода
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Создание динамической библиотеки scanner_core
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
    md5/md5.h
    md5/md5.cpp
)

target_include_directories(scanner_core PRIVATE ${CMAKE_SOURCE_DIR}/md5)

# Создание исполняемого файла scanner
add_executable(scanner
    main.cpp
)

target_link_libraries(scanner PRIVATE scanner_core)

target_include_directories(scanner PRIVATE ${CMAKE_SOURCE_DIR}/include)
