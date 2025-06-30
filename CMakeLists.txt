cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_MODULE_PATH}/${TARGET}.cmake")

# Define project
project(objc-runtime 
    LANGUAGES C OBJC 
    DESCRIPTION "Objective C runtime"
)

# if runtime is gcc, include runtime-gcc
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

include(CTest)
add_compile_options(-Wall -Wextra)
add_compile_options("$<$<CONFIG:DEBUG>:-DDEBUG>")
add_subdirectory(src/runtime-gcc)        
add_subdirectory(src/tests)
