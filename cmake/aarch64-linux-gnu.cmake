# arm64/linux target toolchain file

set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_PROCESSOR "aarch64")

# Use CC environment variable if set, otherwise default to gcc then clang
if(DEFINED ENV{CC})
    find_program(CMAKE_C_COMPILER NAMES "$ENV{CC}"
        PATHS ENV TOOLCHAIN_PATH
        PATH_SUFFIXES bin
        REQUIRED
    )
    find_program(CMAKE_OBJC_COMPILER NAMES "$ENV{CC}"
        PATHS ENV TOOLCHAIN_PATH
        PATH_SUFFIXES bin
        REQUIRED
    )
else()
    find_program(CMAKE_C_COMPILER NAMES "gcc" "clang"
        PATHS ENV TOOLCHAIN_PATH
        PATH_SUFFIXES bin
        REQUIRED
    )
    find_program(CMAKE_OBJC_COMPILER NAMES "gcc" "clang"
        PATHS ENV TOOLCHAIN_PATH
        PATH_SUFFIXES bin
        REQUIRED
    )
endif()

# Compile options for Objective-C
add_compile_options(
    # Use appropriate runtime flags based on compiler
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:Clang>>:-fobjc-runtime=gcc>
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:GNU>>:-fgnu-runtime>
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-exceptions>
    # Use appropriate target flags based on compiler
    $<$<C_COMPILER_ID:Clang>:--target=aarch64-linux-gnu>
)
