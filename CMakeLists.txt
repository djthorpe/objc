cmake_minimum_required(VERSION 3.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
# Expect CMAKE_TOOLCHAIN_FILE to be provided explicitly by build system. If TARGET given
# and toolchain file not already set (e.g. direct invocation), derive it for convenience.
if(NOT CMAKE_TOOLCHAIN_FILE AND DEFINED TARGET)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_MODULE_PATH}/${TARGET}.cmake" CACHE PATH "Toolchain file" FORCE)
endif()
set(PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/boards")

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

# Runtime Libraries
add_subdirectory("src/malloc")
add_subdirectory("src/runtime-sys/${SYSTEM_NAME_LOWER}")
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
