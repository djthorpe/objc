cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_MODULE_PATH}/${TARGET}.cmake")

# Define project
project(objc-runtime 
    LANGUAGES C OBJC 
    DESCRIPTION "Objective C runtime and libraries"
)

# Runtime
if(RUNTIME STREQUAL "gcc")
    message(STATUS "objc runtime is gcc")    
else()
    message(FATAL_ERROR "Unsupported runtime: ${RUNTIME}")
endif()

# Compile options
add_compile_options(
    "-std=c99"
    "-Wall"
    "-Wextra"
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    "$<$<CONFIG:RELEASE>:-O3>"
)

# Libraries
add_subdirectory("src/runtime-${RUNTIME}")
add_subdirectory("src/malloc")
add_subdirectory("src/NXFoundation")

# Tests
include(CTest)
add_subdirectory("src/tests")
