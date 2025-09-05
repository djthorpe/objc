
cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Derive project version from git (fallback 0.0.0).
# Allow manual override via -D PROJECT_VERSION=...
if(NOT DEFINED PROJECT_VERSION)
    find_package(Git QUIET)
    if(GIT_FOUND)
        execute_process(
            COMMAND ${GIT_EXECUTABLE} describe --tags --dirty --always
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE _GIT_DESCRIBE
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
        if(_GIT_DESCRIBE MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+.*")
            set(PROJECT_VERSION "${_GIT_DESCRIBE}")
        else()
            # Attempt to transform e.g. v1.2.3-4-g<hash> -> 1.2.3+4
            if(_GIT_DESCRIBE MATCHES "v?([0-9]+\\.[0-9]+\\.[0-9]+)-([0-9]+)-g[0-9a-f]+")
                set(PROJECT_VERSION "${CMAKE_MATCH_1}+${CMAKE_MATCH_2}")
            endif()
        endif()
    endif()
    if(NOT PROJECT_VERSION)
        set(PROJECT_VERSION "0.0.0")
    endif()
endif()

# Logic if building for a Pico board
if(DEFINED PICO_BOARD)
    # Point to the shared boards header directory at the root of the objc
    # repository before including pico_sdk_init
    set(PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/boards")

    # Include Pico SDK modules (only when targeting a Pico board)
    include(third_party/pico-sdk/pico_sdk_init.cmake)

    # Test programs should make a static library, not an executable
    set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
endif()

# Define the project (uses PROJECT_VERSION if set above)
project(objc-runtime VERSION ${PROJECT_VERSION} LANGUAGES C CXX ASM OBJC)

# Initialise the Pico SDK
if(DEFINED PICO_BOARD)
    pico_sdk_init()

    # Ensure Objective-C compilation uses the same cross-compilation flags as C.
    # The Pico SDK/toolchain injects target triple, cpu, sysroot, float/dsp flags into C/CXX/ASM
    # but does not automatically propagate them to the OBJC language. Without these
    # flags the Objective-C objects are built for the host and become "incompatible"
    # at link time with the embedded objects from the Pico SDK.
    foreach(cfg "" DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
        if(cfg STREQUAL "")
            set(_c_var CMAKE_C_FLAGS)
            set(_objc_var CMAKE_OBJC_FLAGS)
        else()
            string(TOUPPER ${cfg} _ucfg)
            set(_c_var CMAKE_C_FLAGS_${_ucfg})
            set(_objc_var CMAKE_OBJC_FLAGS_${_ucfg})
        endif()
        if(DEFINED ${_c_var})
            # Prepend C flags if OBJC variant lacks a target triple (heuristic: --target=)
            if(NOT DEFINED ${_objc_var} OR NOT ${_objc_var} MATCHES "--target=")
                set(${_objc_var} "${${_c_var}} ${${_objc_var}}")
            endif()
        endif()
    endforeach()
    unset(_c_var)
    unset(_objc_var)
endif()

# Lowercase and uppercase system name
# SYSTEM_NAME_PICO // SYSTEM_NAME=pico
# SYSTEM_NAME_DARWIN // SYSTEM_NAME=darwin
# SYSTEM_NAME_LINUX // SYSTEM_NAME=linux
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)
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

# Compile options for Objective-C
add_compile_options(
    # Use appropriate runtime flags based on compiler
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:Clang>>:-fobjc-runtime=gcc>
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:GNU>>:-fgnu-runtime>
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-exceptions>
)

# Runtime Libraries
add_subdirectory("src/malloc")
add_subdirectory("src/runtime-${RUNTIME}")
add_subdirectory("src/runtime-sys/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/runtime-hw/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/runtime-net/${SYSTEM_NAME_LOWER}")
add_subdirectory("src/drivers")

# Objective-C
add_subdirectory("src/Foundation")
add_subdirectory("src/Application")
add_subdirectory("src/Network")

# Tests
include(CTest)
add_subdirectory("src/tests")

# Examples
add_subdirectory("src/examples")

# Install target
include(cmake/install.cmake)

