# RP2040 target toolchain file

set(CMAKE_SYSTEM_NAME "PICO")
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
add_compile_options(-fobjc-runtime=gcc)
add_compile_options(--target=armv6m-none-eabi)
add_compile_options(-mfloat-abi=soft)
add_compile_options(-march=armv6m)
