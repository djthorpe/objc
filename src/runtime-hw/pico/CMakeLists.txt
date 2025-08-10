set(NAME "runtime-hw")

set(PICO_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}")

# If building for RP2350 with clang, disable software spin locks before pico_sdk_init
if(DEFINED ENV{PICO_BOARD} AND "$ENV{PICO_BOARD}" MATCHES "rp2350" AND CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(STATUS "(runtime-hw) Disabling software spin locks for RP2350 + Clang build (PICO_USE_SW_SPIN_LOCKS=0)")
    set(PICO_USE_SW_SPIN_LOCKS 0 CACHE BOOL "Disable SW spin locks for RP2350 clang" FORCE)
endif()

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
    power.c
    pwm.c
    spi.c
    watchdog.c
    ${CMAKE_CURRENT_BINARY_DIR}/infrared_rx.pio.h
)
target_include_directories(${NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_LIST_DIR}
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

# Check if we're building for a board with CYW43 support
if (PICO_CYW43_SUPPORTED)
    message(STATUS "Board has CYW43 support - enabling CYW43 and lwIP libraries")
    target_compile_definitions(${NAME} PRIVATE PICO_CYW43_SUPPORTED=1)
    target_link_libraries(${NAME} 
        pico_cyw43_arch_lwip_poll 
    )
endif()
