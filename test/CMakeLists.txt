cmake_minimum_required(VERSION 3.21)

project(extract_text)

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_search_module(TESSERACT REQUIRED tesseract)
pkg_search_module(LEPTONICA REQUIRED lept)
#
include_directories(${TESSERACT_INCLUDE_DIRS})
include_directories(${LEPTONICA_INCLUDE_DIRS})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

add_executable(${PROJECT_NAME} main.cpp wer.hpp)

target_link_libraries(${PROJECT_NAME} stdc++fs)
target_link_libraries(${PROJECT_NAME} pthread curl)
target_link_libraries(${PROJECT_NAME} tesseract pthread curl)
target_link_libraries(${PROJECT_NAME} ${LEPTONICA_LIBRARIES})
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libleptonica.so)
target_link_libraries(${PROJECT_NAME} ${TESSERACT_LIBRARIES})

