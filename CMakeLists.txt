
cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(DEFINED PICO_BOARD)
    # Point to the shared boards header directory at the root of the objc
    # repository before including pico_sdk_init
    set(PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/boards")

    # Include Pico SDK modules (only when targeting a Pico board)
    include(third_party/pico-sdk/pico_sdk_init.cmake)

    # Test programs should make a static library, not an executable
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
endif()

# Define the project
project(objc-runtime C CXX ASM OBJC)

if(DEFINED PICO_BOARD)
    # Initialise the Pico SDK (only if Pico SDK was included)
    pico_sdk_init()

    # Propagate certain board-provided CMake variables as global compile definitions
    # (acts as a hook so that comment-based pico_cmake_set lines in board headers
    # become visible to C/C++ code without adding fallback #defines in headers)
    if(DEFINED PICO_CYW43_SUPPORTED AND PICO_CYW43_SUPPORTED)
        add_compile_definitions(PICO_CYW43_SUPPORTED=1)
    endif()
endif()

# Lowercase and uppercase system name
# SYSTEM_NAME_PICO // SYSTEM_NAME=pico
# SYSTEM_NAME_DARWIN // SYSTEM_NAME=darwin
# SYSTEM_NAME_LINUX // SYSTEM_NAME=linux
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)

# Compile options
add_compile_options(
    "-Wall"
    "-Wextra"
    "$<$<CONFIG:DEBUG>:-DDEBUG>"
    "$<$<CONFIG:RELEASE>:-O3>"
    "-DSYSTEM_NAME_${SYSTEM_NAME_UPPER}"
    "-DSYSTEM_NAME=\"${SYSTEM_NAME_LOWER}\""
)

# Compile options for Objective-C
add_compile_options(
    # Use appropriate runtime flags based on compiler
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:Clang>>:-fobjc-runtime=gcc>
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:GNU>>:-fgnu-runtime>
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-exceptions>
)

# Runtime Libraries
add_subdirectory("src/malloc")
add_subdirectory("src/runtime-sys/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/runtime-hw/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/runtime-${RUNTIME}")
add_subdirectory("src/drivers")

# Tests
include(CTest)
add_subdirectory("src/tests")

# Examples
add_subdirectory("src/examples")
