# this is included because toolchain file sets SYSTEM_NAME=Pico

set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)
set(CMAKE_EXECUTABLE_SUFFIX .elf)
