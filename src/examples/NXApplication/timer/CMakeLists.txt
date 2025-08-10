set(NAME "nxapplication_timer")
add_executable(${NAME}
    main.m
    hack.c
)
target_link_libraries(${NAME} PRIVATE
    NXApplication
)

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "Pico")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()
