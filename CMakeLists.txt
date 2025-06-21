# Invoke with 
# cmake -B build

# Versions
cmake_minimum_required(VERSION 3.18)

# Define project
project(fuse-objc LANGUAGES C CXX ASM DESCRIPTION "objc")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
set(CMAKE_ASM_COMPILER "clang")
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang")
set(CMAKE_BUILD_TYPE Debug)

#add_subdirectory(src/objc)
#add_subdirectory(src/test)
add_subdirectory(src/helloworld)
