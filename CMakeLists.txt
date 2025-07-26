cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_MODULE_PATH}/${TARGET}.cmake")

# Define project
project(objc-runtime 
    LANGUAGES C OBJC ASM CXX
    DESCRIPTION "Objective C runtime and libraries"
)

# Runtime
if(RUNTIME STREQUAL "gcc")
    message(STATUS "objc runtime is gcc")    
else()
    message(FATAL_ERROR "Unsupported runtime: ${RUNTIME}")
endif()

# Set C and C++ standards properly
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS ON)  # Enable GNU extensions for Pico SDK compatibility
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

# Lowercase and uppercase system name
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)

# Compile options
add_compile_options(
    "-Wall"
    "-Wextra"
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    "$<$<CONFIG:RELEASE>:-O3>"
    "-DSYSTEM_NAME_${SYSTEM_NAME_UPPER}"
)

# Libraries
add_subdirectory("src/malloc")
add_subdirectory("src/runtime-sys/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/runtime-${RUNTIME}")
add_subdirectory("src/NXFoundation")

# Tests
include(CTest)
add_subdirectory("src/tests")
