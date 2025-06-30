# ARM/Darwin target toolchain file

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "x86-64")

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
add_compile_options(-fobjc-exceptions)
add_compile_options(--target=x86_64-linux-gnu)
add_compile_options(-march=x86-64)

