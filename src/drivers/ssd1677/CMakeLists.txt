# SSD1677 E-Paper Display Driver
set(SSD1677_SOURCES
    ssd1677.c
)

# Create the SSD1677 driver library
add_library(ssd1677 ${SSD1677_SOURCES})

# Link dependencies
target_link_libraries(ssd1677
    runtime-sys
    runtime-hw
)

# Set include directories
target_include_directories(ssd1677 PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Compiler-specific flags
if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(ssd1677 PRIVATE
        -Wno-unused-parameter
        -Wno-unused-variable
    )
endif()