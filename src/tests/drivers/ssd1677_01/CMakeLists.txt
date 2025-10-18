set(NAME "ssd1677_01")
add_executable(${NAME}
    main.c
)
target_link_libraries(${NAME} PRIVATE
    drivers
    ssd1677
    malloc
    runtime-sys
)

# Include directories
target_include_directories(${NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

add_test(NAME ${NAME} COMMAND ${NAME})

if(CMAKE_SYSTEM_NAME STREQUAL "Pico")
pico_add_extra_outputs(${NAME})
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)
endif()