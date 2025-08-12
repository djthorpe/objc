
cmake_minimum_required(VERSION 3.27)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Point to the shared boards header directory at the root of the objc repository BEFORE including pico_sdk_init
set(PICO_BOARD_HEADER_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../../include/boards")

include(../../../../third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

project(hello_usb C CXX ASM)

#########################################

add_executable(hello_usb main.c)
target_link_libraries(hello_usb pico_stdlib)

# enable usb output, disable uart output
pico_enable_stdio_usb(hello_usb 1)
pico_enable_stdio_uart(hello_usb 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(hello_usb)

#########################################
