# RP2040 target toolchain file

set(CMAKE_SYSTEM_NAME "Pico")
set(CMAKE_SYSTEM_PROCESSOR "cortex-m0plus")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

find_program(CMAKE_C_COMPILER NAMES "clang"
    PATHS ENV TOOLCHAIN_PATH
    PATH_SUFFIXES bin
    NO_DEFAULT_PATH
    REQUIRED
)
find_program(CMAKE_OBJC_COMPILER NAMES "clang"
    PATHS ENV TOOLCHAIN_PATH
    PATH_SUFFIXES bin
    NO_DEFAULT_PATH
    REQUIRED
)


# Compile options for Objective-C
add_compile_options(
    # Use appropriate runtime flags based on compiler
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:Clang>>:-fobjc-runtime=gcc>
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:GNU>>:-fgnu-runtime>
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-exceptions>
    # Use appropriate target flags based on compiler
    $<$<C_COMPILER_ID:Clang>:--target=armv6m-none-eabi>
    $<$<C_COMPILER_ID:GNU>:-march=armv6m>
)
