set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(minisql)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

find_package(BISON 3.0.0 REQUIRED)
find_package(FLEX REQUIRED)

BISON_TARGET(interpreter_parser
        ${CMAKE_SOURCE_DIR}/src/Interpreter/interpreter.y
        ${CMAKE_CURRENT_BINARY_DIR}/Parser.cpp
        COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/Parser.hpp --verbose")
FLEX_TARGET(interpreter_lexer
        ${CMAKE_SOURCE_DIR}/src/Interpreter/interpreter.l
        ${CMAKE_CURRENT_BINARY_DIR}/Scanner.cpp)
ADD_FLEX_BISON_DEPENDENCY(interpreter_lexer interpreter_parser)

include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_BINARY_DIR})

file(GLOB SOURCE_FILES
        "src/main.cpp"
        "src/*.hpp"
        "src/CatalogManager/*.hpp"
        "src/CatalogManager/*.cpp"
        "src/BufferManager/*.hpp"
        "src/BufferManager/*.cpp"
        "src/FileManager/*.hpp"
        "src/FileManager/*.cpp"
        "src/IndexManager/*.hpp"
        "src/IndexManager/*.cpp"
        "src/RecordManager/*.hpp"
        "src/RecordManager/*.cpp"
        "src/Interpreter/Scanner.hpp"
        "src/Interpreter/Driver.hpp"
        "src/Interpreter/Driver.cpp"
        "src/Type/*.hpp"
        "src/Type/*.cpp"
        "src/API/*.hpp"
        "src/API/*.cpp"
        "src/utils/*.hpp"
        "src/utils/*.cpp"
        )

add_executable(${CMAKE_PROJECT_NAME}
        ${SOURCE_FILES}
        ${FLEX_interpreter_lexer_OUTPUTS}
        ${BISON_interpreter_parser_OUTPUTS})

target_include_directories(${CMAKE_PROJECT_NAME}
        PRIVATE
        .
        ${CMAKE_CURRENT_BINARY_DIR})

set_property(TARGET ${CMAKE_PROJECT_NAME}
        PROPERTY CXX_STANDARD 14)
