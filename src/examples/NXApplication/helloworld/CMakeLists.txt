set(NAME "nxapplication_helloworld")
add_executable(${NAME}
    main.m
    hack.c
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
    runtime-sys
    runtime-hw
    NXFoundation
    NXApplication
)

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
    pico_add_extra_outputs(${NAME})
    pico_enable_stdio_usb(${NAME} 1)
    pico_enable_stdio_uart(${NAME} 0)
endif()
