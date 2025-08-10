
cmake_minimum_required(VERSION 3.27)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Point to the shared boards header directory at the root of the objc
# repository before including pico_sdk_init
set(PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/boards")

# Initialise the Pico SDK
include(third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

# Define the project
project(objc-runtime C CXX ASM)

# Propagate certain board-provided CMake variables as global compile definitions
# (acts as a hook so that comment-based pico_cmake_set lines in board headers
# become visible to C/C++ code without adding fallback #defines in headers)
if(DEFINED PICO_CYW43_SUPPORTED AND PICO_CYW43_SUPPORTED)
	add_compile_definitions(PICO_CYW43_SUPPORTED=1)
endif()

# Lowercase and uppercase system name
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_UPPER)

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
