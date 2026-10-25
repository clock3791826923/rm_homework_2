cmake_minimum_required(VERSION 3.16)
project(task1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#yaml-cpp 库。
find_package(yaml-cpp REQUIRED)

add_executable(task1 src/task1.cpp)

# 把 yaml-cpp 链接到 task1 上
if(TARGET yaml-cpp::yaml-cpp)
    target_link_libraries(task1 PRIVATE yaml-cpp::yaml-cpp)
else()
    target_link_libraries(task1 PRIVATE yaml-cpp)
endif()