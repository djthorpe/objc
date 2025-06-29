cmake_minimum_required(VERSION 3.10)

# Define project
project(objc-gcc C OBJC)

# if runtime is gcc, include runtime-gcc
if(RUNTIME STREQUAL "gcc")
    message(STATUS "objc runtime is gcc")    
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-fgnu-runtime)
    elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-fobjc-runtime=gcc)
    elseif(CMAKE_C_COMPILER_ID STREQUAL "AppleClang")
        add_compile_options(-fobjc-runtime=gcc)
    endif()
else()
    message(FATAL_ERROR "Unsupported runtime: ${RUNTIME}")
endif()

include(CTest)
add_compile_options(-Wall -Wextra)
add_subdirectory(src/runtime-gcc)        
add_subdirectory(src/tests)
