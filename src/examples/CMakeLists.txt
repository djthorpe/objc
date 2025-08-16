
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
add_subdirectory(runtime/clock)
add_subdirectory(runtime/multicore)
add_subdirectory(runtime/simplequeue)
add_subdirectory(runtime/runloop)
add_subdirectory(runtime/wifiscan)
add_subdirectory(runtime/wificonnect)

# Driver examples
add_subdirectory(drivers/bme280)
add_subdirectory(drivers/uc8151)
add_subdirectory(drivers/st7789)

# Application examples
add_subdirectory(Application/helloworld)
add_subdirectory(Application/gpio)
add_subdirectory(Application/timer)
add_subdirectory(Application/blink)

# Network examples
add_subdirectory(Network/scan)
add_subdirectory(Network/connect)
