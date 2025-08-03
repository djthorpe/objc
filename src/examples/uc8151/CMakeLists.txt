set(NAME "uc8151")
add_executable(${NAME}
    main.c
)
target_link_libraries(${NAME} PRIVATE
    runtime-sys
    runtime-hw
    drivers
)

# Partial update test executable
set(PARTIAL_NAME "uc8151_partial_test")
add_executable(${PARTIAL_NAME}
    partial_test.c
)
target_link_libraries(${PARTIAL_NAME} PRIVATE
    runtime-sys
    runtime-hw
    drivers
)

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "Pico")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

pico_add_extra_outputs(${PARTIAL_NAME})
pico_enable_stdio_usb(${PARTIAL_NAME} 1)
pico_enable_stdio_uart(${PARTIAL_NAME} 0)
endif()
