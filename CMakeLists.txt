# Versions
cmake_minimum_required(VERSION 3.18)

# Define project
project(fuse-objc VERSION 0.0.1 LANGUAGES C ASM)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(src/objc)
add_subdirectory(src/test)
