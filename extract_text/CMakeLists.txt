cmake_minimum_required(VERSION 3.17)
project(extract_text)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_search_module(TESSERACT REQUIRED tesseract)
pkg_search_module(LEPTONICA REQUIRED lept)
#
include_directories(${TESSERACT_INCLUDE_DIRS})
include_directories(${LEPTONICA_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libxl-4.0.0.0/include_cpp)

add_executable(${PROJECT_NAME} main.cpp wer.hpp)

target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(${PROJECT_NAME} pthread curl)
target_link_libraries(${PROJECT_NAME} tesseract pthread curl)
target_link_libraries(${PROJECT_NAME} ${LEPTONICA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} /usr/local/lib/liblept.so.5)
target_link_libraries(${PROJECT_NAME} ${TESSERACT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/libxl-4.0.0.0/lib64/libxl.so)
