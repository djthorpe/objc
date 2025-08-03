set(NAME "runtime-hw")

set(PICO_COMPILER "pico_arm_gcc")
set(PICO_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}")
include(../../../third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

add_library(${NAME} STATIC
    adc.c
    gpio.c
    hw.c
    i2c.c
    pwm.c
    spi.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
target_link_libraries(${NAME} 
    hardware_adc
    hardware_clocks
    hardware_gpio
    hardware_i2c
    hardware_pwm
    hardware_spi
)
