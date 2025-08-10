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

# Lowercase processor name
string(TOLOWER ${CMAKE_SYSTEM_PROCESSOR} PROCESSOR_NAME_LOWER)

# Compile options
add_compile_options(
    "-Wall"
    "-Wextra"
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    "$<$<CONFIG:RELEASE>:-O3>"
    "-DSYSTEM_NAME_${SYSTEM_NAME_UPPER}"
    "-DSYSTEM_NAME=\"${SYSTEM_NAME_LOWER}\""
    "-DSYSTEM_PROCESSOR=\"${PROCESSOR_NAME_LOWER}\""
)

# For RP2350 with clang, disable software spin locks which aren't supported
if(CMAKE_SYSTEM_NAME STREQUAL "Pico" AND CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_compile_definitions(PICO_USE_SW_SPIN_LOCK=0)
endif()

# Runtime Libraries
add_subdirectory("src/malloc")
add_subdirectory("src/runtime-sys/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/runtime-pix")
add_subdirectory("src/runtime-hw/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/runtime-${RUNTIME}")
add_subdirectory("src/drivers")

# Objective-C Libraries
add_subdirectory("src/NXFoundation")
add_subdirectory("src/NXApplication")

# Tests
include(CTest)
add_subdirectory("src/tests")

# Examples
add_subdirectory("src/examples")
