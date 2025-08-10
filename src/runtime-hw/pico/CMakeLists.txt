set(NAME "runtime-hw")

set(PICO_COMPILER "pico_arm_gcc")
set(PICO_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}")
include(../../../third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

# Generate PIO headers
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/infrared_rx.pio.h
    COMMAND ${CMAKE_CURRENT_LIST_DIR}/../../../build/pioasm/pioasm 
            ${CMAKE_CURRENT_LIST_DIR}/infrared_rx.pio
            ${CMAKE_CURRENT_BINARY_DIR}/infrared_rx.pio.h
    DEPENDS ${CMAKE_CURRENT_LIST_DIR}/infrared_rx.pio
    COMMENT "Generating PIO header for IR receiver"
)

add_library(${NAME} STATIC
    adc.c
    gpio.c
    hw.c
    i2c.c
    infrared_rx.c
    led.c
    pwm.c
    spi.c
    watchdog.c
    ${CMAKE_CURRENT_BINARY_DIR}/infrared_rx.pio.h
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
    ${CMAKE_CURRENT_BINARY_DIR}
)
target_link_libraries(${NAME} 
    hardware_adc
    hardware_clocks
    hardware_gpio
    hardware_i2c
    hardware_pio
    hardware_pwm
    hardware_spi
    hardware_timer
)
