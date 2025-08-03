
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

add_subdirectory(app)
add_subdirectory(clock)
add_subdirectory(bme280)
add_subdirectory(pico/multicore)
add_subdirectory(pico/simplequeue)
add_subdirectory(pico/runloop)
add_subdirectory(pico/gpio_runloop)
