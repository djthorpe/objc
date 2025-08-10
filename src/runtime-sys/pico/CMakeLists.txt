set(NAME "runtime-sys")

# If building for RP2350 with clang, disable software spin locks (PICO_USE_SW_SPIN_LOCKS)
# The Pico SDK defaults these to 1 on RP2350 due to errata with hardware spinlocks,
# but the software implementation currently lacks clang support in our configuration
# leading to 'no SW_SPIN_TRY_LOCK available' errors. This must be set BEFORE pico_sdk_init().
if(DEFINED ENV{PICO_BOARD} AND "$ENV{PICO_BOARD}" MATCHES "rp2350" AND CMAKE_C_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Disabling software spin locks for RP2350 + Clang build (PICO_USE_SW_SPIN_LOCKS=0)")
    set(PICO_USE_SW_SPIN_LOCKS 0 CACHE BOOL "Disable SW spin locks for RP2350 clang" FORCE)
endif()

set(PICO_TOOLCHAIN_PATH "${TOOLCHAIN_PATH}")
include(../../../third_party/pico-sdk/pico_sdk_init.cmake)
pico_sdk_init()

add_library(${NAME} STATIC
    abort.c
    cond.c
    date.c
    env.c
    hash.c
    memory.c
    mutex.c
    puts.c
    random.c
    sys.c
    timer.c
    thread.c
    waitgroup.c
    ../all/date.c
    ../all/event.c
    ../all/hash.c
    ../all/hashtable.c
    ../all/mutex.c
    ../all/panicf.c
    ../all/printf.c
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
    pico_multicore
    pico_standard_binary_info
)

# Propagate spin lock choice to compilation units
if(DEFINED PICO_USE_SW_SPIN_LOCKS)
    target_compile_definitions(${NAME} PUBLIC PICO_USE_SW_SPIN_LOCKS=${PICO_USE_SW_SPIN_LOCKS})
endif()
