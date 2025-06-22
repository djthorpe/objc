
# Compile pico version of helloworld
set(NAME "helloworld-armv6m-pico")
add_executable(${NAME}
    main-armv6m-pico.s
)
target_link_libraries(${NAME} pico_stdlib)
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)