# Invoke with 
# cmake -B build

# Versions
cmake_minimum_required(VERSION 3.18)

# Define project
project(fuse-objc VERSION 0.0.1 LANGUAGES C ASM)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_C_FLAGS "-fobjc-runtime=macosx")

#add_subdirectory(src/objc)
#add_subdirectory(src/test)
add_subdirectory(src/helloworld)
