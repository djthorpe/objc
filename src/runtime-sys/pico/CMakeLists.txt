set(NAME "runtime-sys")

set(PICO_COMPILER "pico_arm_gcc")
set(PICO_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}")
include(../../../third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

add_library(${NAME} STATIC
    abort.c
    hash.c
    memory.c
    mutex.c
    puts.c
    random.c
    sys.c
    timeget.c
    timer.c
    thread.c
    ../all/event.c
    ../all/panicf.c
    ../all/printf.c
    ../posix/time.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../../include/runtime-${RUNTIME}
)
target_include_directories(${NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)
target_link_libraries(${NAME} 
    pico_stdlib
    pico_rand
    pico_time
    pico_mbedtls
)
