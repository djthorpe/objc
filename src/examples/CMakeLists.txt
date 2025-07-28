
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# Pico Examples
add_subdirectory(pico/multicore)
add_subdirectory(pico/simplequeue)
