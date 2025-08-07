
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# NXApplication examples
add_subdirectory(NXApplication/helloworld)
add_subdirectory(NXApplication/gpio)
add_subdirectory(NXApplication/timer)

# Runtime examples
add_subdirectory(runtime/watchdog)
add_subdirectory(runtime/led)

# Unorganized examples - to be organized later!
add_subdirectory(clock)
add_subdirectory(bme280)
add_subdirectory(uc8151)
add_subdirectory(st7789)
add_subdirectory(pico/multicore)
add_subdirectory(pico/simplequeue)
add_subdirectory(pico/runloop)
add_subdirectory(pico/gpio_runloop)
add_subdirectory(pico/pwm_blink)