set(NAME "runtime_power")
add_executable(${NAME}
    main.c
    hack.c
)
target_link_libraries(${NAME} PRIVATE
    runtime-hw
)

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()
