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


# Compile options for Objective-C GCC
add_compile_options(
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-runtime=gcc>
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-exceptions>    
    --target=x86_64-linux-gnu
    -march=x86-64
)
