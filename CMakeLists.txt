set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(minisql)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

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
        "src/IndexManger/*.hpp"
        "src/IndexManger/*.cpp"
        "src/RecordManager/*.hpp"
        "src/RecordManager/*.cpp"
        "src/Type/*.hpp"
        "src/Type/*.cpp"
        "src/API/*.hpp"
        "src/API/*.cpp"
        "src/utils/*.hpp"
        "src/utils/*.cpp"
        )

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
