set(NAME "runtime-sys")

set(PICO_COMPILER "pico_arm_clang")
set(PICO_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}")
include(../../../third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

add_library(${NAME} STATIC
    abort.c
    ../all/panicf.c
    ../all/printf.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
target_link_libraries(${NAME} pico_stdlib)
