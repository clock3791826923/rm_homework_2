cmake_minimum_required(VERSION 3.16)
project(task4_simple LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Threads REQUIRED)

add_executable(task4 src/task4.cpp)

target_link_libraries(task4 PRIVATE Threads::Threads)

if(MSVC)
    target_compile_options(task4 PRIVATE /utf-8)
endif()
