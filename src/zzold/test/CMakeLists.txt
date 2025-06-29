set(NAME "test")

add_executable(${NAME}
    main.m
    Test.m
)
target_compile_options(${NAME} PRIVATE
    -fobjc-runtime=gcc
    -Wno-unused-command-line-argument
)
target_link_options(${NAME} PRIVATE
    -fobjc-runtime=gcc
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
)
if (DEFINED PICO_SDK_PATH)
    target_link_libraries(${NAME} PRIVATE pico_stdlib)
    pico_enable_stdio_usb(${NAME} 1)
    pico_enable_stdio_uart(${NAME} 0)
    pico_add_extra_outputs(${NAME})
endif()
