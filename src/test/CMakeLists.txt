set(NAME "test")

add_executable(${NAME}
    main.m
    Test.m
)
target_compile_options(${NAME} PRIVATE
    -fobjc-runtime=macosx
    -Wno-unused-command-line-argument
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} LINK_PUBLIC
    objc
    pico_stdlib
)
pico_enable_stdio_usb(${NAME} 1)
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex/uf2 files
pico_add_extra_outputs(${NAME})
