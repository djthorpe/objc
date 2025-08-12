set(NAME "runloop")
add_executable(${NAME}
    main.c
    hack.c
)
target_link_libraries(${NAME} PRIVATE
    runtime-sys
)

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "Pico")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()
