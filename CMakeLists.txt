cmake_minimum_required(VERSION 3.13...3.27)
set(PICO_COMPILER "pico_arm_clang")
set(PICO_TOOLCHAIN_PATH "/Applications/LLVM-ET-Arm-19.1.5-Darwin-universal/bin")
include(third_party/pico-sdk/pico_sdk_init.cmake)

# Define project
project(fuse-objc CXX C ASM)

# Pico support
pico_sdk_init()
add_subdirectory(src/test2)
