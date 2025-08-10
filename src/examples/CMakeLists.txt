
# add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include/runtime-${RUNTIME}
)

# Runtime examples
add_subdirectory(runtime/helloworld)
add_subdirectory(runtime/watchdog)
add_subdirectory(runtime/led)
add_subdirectory(runtime/infrared)
add_subdirectory(runtime/pwm)
add_subdirectory(runtime/power)
add_subdirectory(runtime/gpio)

# NXApplication examples
#add_subdirectory(NXApplication/helloworld)
#add_subdirectory(NXApplication/gpio)
#add_subdirectory(NXApplication/timer)

# Unorganized examples - to be organized later!
add_subdirectory(clock)
add_subdirectory(bme280)
add_subdirectory(uc8151)
add_subdirectory(st7789)
add_subdirectory(pico/multicore)
add_subdirectory(pico/simplequeue)
add_subdirectory(pico/runloop)
