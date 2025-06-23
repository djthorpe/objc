cmake_minimum_required(VERSION 3.13...3.27)

message(STATUS "PICO_BOARD: ${PICO_BOARD}")

# Define targets
if (PICO_BOARD STREQUAL "amd64-linux-gnu")
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_ASM_COMPILER "clang")
    add_compile_options("-target" "x86_64-linux-gnu")
elseif(PICO_BOARD STREQUAL "arm64-linux-gnu")
    set(CMAKE_C_COMPILER "clang")
    set(CMAKE_ASM_COMPILER "clang")
    add_compile_options("-target" "arm64-linux-gnu")
else()
    # Check toolchain path
    if (NOT EXISTS ${LLVM_TOOLCHAIN_PATH})
        message(FATAL_ERROR "LLVM_TOOLCHAIN_PATH is not set or is invalid. Please set it to the path of your LLVM toolchain.")
    endif()

    # Use clang
    set(PICO_COMPILER "pico_arm_clang")
    set(PICO_TOOLCHAIN_PATH "${LLVM_TOOLCHAIN_PATH}")
    #set(PICOTOOL_FETCH_FROM_GIT_PATH "${CMAKE_CURRENT_LIST_DIR}/third_party")
    include(third_party/pico-sdk/pico_sdk_init.cmake)
endif()

# Define project
project(fuse-objc C ASM)

# Pico support
if (DEFINED PICO_SDK_PATH)
    pico_sdk_init()
endif()

# Build objc runtime
add_subdirectory(src/objc-gcc)

# Build test application
add_subdirectory(src/test)
