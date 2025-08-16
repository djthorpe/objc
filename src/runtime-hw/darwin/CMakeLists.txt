set(NAME "runtime-hw")

add_library(${NAME} STATIC
    ../stub/adc.c
    ../stub/gpio.c
    ../stub/hw.c
    ../stub/i2c.c
    ../stub/infrared_rx.c
    ../stub/led.c
    ../stub/power.c
    ../stub/pwm.c
    ../stub/spi.c
    ../stub/watchdog.c
    ../stub/wifi.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
target_link_libraries(${NAME} 
    runtime-sys
)
