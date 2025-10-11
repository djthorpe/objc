set(NAME "runtime-hw")

# Find libgpiod using pkg-config
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(LIBGPIOD QUIET libgpiod>=2.0)
endif()

# Select GPIO implementation based on libgpiod availability
if(LIBGPIOD_FOUND)
    set(GPIO_SOURCE libgpiod/gpio.c)
    message(STATUS "Using libgpiod for GPIO support")
else()
    set(GPIO_SOURCE ../stub/gpio.c)
    message(STATUS "Using stub implementation for GPIO (libgpiod not found)")
endif()

add_library(${NAME} STATIC
    ../stub/adc.c
    ${GPIO_SOURCE}
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

# Link libraries
target_link_libraries(${NAME} PRIVATE
    runtime-sys
)

# Add libgpiod includes and libraries if found
if(LIBGPIOD_FOUND)
    target_include_directories(${NAME} PRIVATE ${LIBGPIOD_INCLUDE_DIRS})
    target_compile_options(${NAME} PRIVATE ${LIBGPIOD_CFLAGS_OTHER})
    target_link_libraries(${NAME} PRIVATE ${LIBGPIOD_LIBRARIES})
    target_link_directories(${NAME} PRIVATE ${LIBGPIOD_LIBRARY_DIRS})
endif()
