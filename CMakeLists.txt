set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(minisql)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_BINARY_DIR})

file(GLOB SOURCE_FILES
        "src/*.cpp"
        "src/*.hpp"
        "src/utils/*.hpp"
        "src/utils/*.cpp"
        )

add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})
