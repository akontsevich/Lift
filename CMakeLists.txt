cmake_minimum_required(VERSION 2.8)
project(Lift)
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

include_directories(
    ${PROJECT_SOURCE_DIR})

find_package(Threads REQUIRED)

set(main.cpp
    lift.cpp
    lift.h)

add_executable(${PROJECT_NAME} "main.cpp" "lift.cpp" "lift.h")
