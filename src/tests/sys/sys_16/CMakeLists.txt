set(NAME "sys_16")
add_executable(${NAME}
    main.c
)
target_link_libraries(${NAME} PRIVATE
    runtime-sys
)
add_test(NAME ${NAME} COMMAND ${NAME})

# additional configurations for the Pico SDK
if(CMAKE_SYSTEM_NAME STREQUAL "Pico")
# Set binary info for the program
set(PICO_PROGRAM_NAME "sys_16_test")
set(PICO_PROGRAM_VERSION_STRING "1.0.0")
set(PICO_PROGRAM_DESCRIPTION "System Environment Test")
set(PICO_PROGRAM_URL "https://github.com/djthorpe/objc")

pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()
