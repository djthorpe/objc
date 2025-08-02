set(NAME "runtime-pix")

# Try to find SDL3 (optional)
find_package(SDL3 QUIET)

# Base source files (always included)
set(SOURCES
    display.c
    pix.c
    rgba32.c
)

# Add SDL3-specific sources if available
if(SDL3_FOUND)
    list(APPEND SOURCES sdl.c)
endif()

add_library(${NAME} STATIC ${SOURCES})
target_include_directories(${NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${NAME} PRIVATE
    runtime-sys
)

# If SDL3 is found, link it and set compiler definition
if(SDL3_FOUND)
    target_link_libraries(${NAME} PRIVATE SDL3::SDL3)
    target_compile_definitions(${NAME} PRIVATE SUPPORTED_SDL3=1)
    message(STATUS "SDL3 found - enabling SDL3 support in runtime-pix")
endif()
