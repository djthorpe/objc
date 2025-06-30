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
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fgnu-runtime)
    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
        add_compile_options(-fobjc-runtime=gcc)
    endif()
else()
    message(FATAL_ERROR "Unsupported runtime: ${RUNTIME}")
endif()

add_compile_options(-Wall -Wextra)
add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")

# Libraries
add_subdirectory("src/runtime-${RUNTIME}")        
add_subdirectory("src/NXFoundation")

# Tests
include(CTest)
add_subdirectory("src/tests")
