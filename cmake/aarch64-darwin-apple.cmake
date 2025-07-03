# ARM/Darwin target toolchain file

set(CMAKE_SYSTEM_NAME "Darwin")
set(CMAKE_SYSTEM_PROCESSOR "arm64")

find_program(CMAKE_C_COMPILER NAMES "gcc-15"
    PATHS ENV TOOLCHAIN_PATH
    PATH_SUFFIXES bin
    NO_DEFAULT_PATH
    REQUIRED
)
find_program(CMAKE_OBJC_COMPILER NAMES "gcc-15"
    PATHS ENV TOOLCHAIN_PATH
    PATH_SUFFIXES bin
    NO_DEFAULT_PATH
    REQUIRED
)

# Compile options for Objective-C GCC
add_compile_options(
    $<$<COMPILE_LANGUAGE:OBJC>:-fgnu-runtime>
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-exceptions>    
)

