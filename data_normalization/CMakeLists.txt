cmake_minimum_required(VERSION 3.10)
project(data_normalization)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(${PROJECT_NAME} pthread)