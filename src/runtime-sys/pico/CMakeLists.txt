set(NAME "runtime-sys")

set(PICO_COMPILER "pico_arm_clang")
set(PICO_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}")
include(../../../third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

add_library(${NAME} STATIC
    abort.c
    puts.c
    sys.c
    timeget.c
    thread.c
    ../all/panicf.c
    ../all/printf.c
    ../posix/time.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../../include/runtime-${RUNTIME}
)
target_link_libraries(${NAME} 
    pico_stdlib
    pico_aon_timer
    pico_rand
)
