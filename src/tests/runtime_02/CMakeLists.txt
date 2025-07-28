set(NAME "runtime_02")
add_executable(${NAME}
    Test.m
    main.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
    runtime-sys
    malloc
)
add_test(NAME ${NAME} COMMAND ${NAME})

if(CMAKE_SYSTEM_NAME STREQUAL "Pico")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()

