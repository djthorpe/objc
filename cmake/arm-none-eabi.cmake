# RP2350 target toolchain file
set(CMAKE_SYSTEM_NAME "Pico")
set(CMAKE_SYSTEM_PROCESSOR "cortex-m33")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Provide .elf suffix for produced executables so picotool can recognize
# the file type when converting to UF2. Force into cache early since setting
# this inside the toolchain file may occur before project() and we want to
# override any platform default (macOS default is empty). Use FORCE to ensure
# it applies even if previously defined.
set(CMAKE_EXECUTABLE_SUFFIX ".elf" CACHE STRING "Executable suffix for Pico cross builds" FORCE)

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
find_program(CMAKE_ASM_COMPILER NAMES "clang"
    PATHS ENV TOOLCHAIN_PATH
    PATH_SUFFIXES bin
    NO_DEFAULT_PATH
    REQUIRED
)

# Objcopy (needed by pico SDK to produce .bin from .elf). Use llvm-objcopy from TOOLCHAIN_PATH.
find_program(CMAKE_OBJCOPY NAMES "llvm-objcopy"
    PATHS ENV TOOLCHAIN_PATH
    PATH_SUFFIXES bin
    NO_DEFAULT_PATH
    REQUIRED
)

# Set the target triple for all languages
set(CMAKE_C_COMPILER_TARGET "arm-none-eabi")
set(CMAKE_OBJC_COMPILER_TARGET "arm-none-eabi")
set(CMAKE_ASM_COMPILER_TARGET "arm-none-eabi")
set(CMAKE_CXX_COMPILER_TARGET "arm-none-eabi")

# Determine architecture flags: default to cortex-m33 if processor hints present, else armv6m
if(CMAKE_SYSTEM_PROCESSOR MATCHES "cortex-m33")
    set(_ARCH_MARCH_FLAG "-mcpu=cortex-m33")
    set(_ARCH_TRIPLE_SUFFIX "")
else()
    # Assume RP2040 style
    set(_ARCH_MARCH_FLAG "-march=armv6m")
    set(_ARCH_TRIPLE_SUFFIX "v6m")
endif()

# Common language compile options
add_compile_options(
    $<$<COMPILE_LANGUAGE:C,CXX,OBJC,ASM>:${_ARCH_MARCH_FLAG}>
)

# Objective-C runtime flags
add_compile_options(
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:Clang>>:-fobjc-runtime=gcc>
    $<$<AND:$<COMPILE_LANGUAGE:OBJC>,$<C_COMPILER_ID:GNU>>:-fgnu-runtime>
    $<$<COMPILE_LANGUAGE:OBJC>:-fobjc-exceptions>
)

# For clang ensure target triple is applied consistently (already set via *_COMPILER_TARGET but be explicit)
add_compile_options(
    $<$<C_COMPILER_ID:Clang>:--target=arm-none-eabi>
)
add_link_options(
    $<$<C_COMPILER_ID:Clang>:--target=arm-none-eabi>
)
