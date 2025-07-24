set(NAME "runtime-sys")

add_library(${NAME} STATIC
    abort.c
    exception.c
    mutex.c
    sys.c
    ../posix/malloc.c
    ../posix/puts.c
    ../posix/random.c
    ../posix/sleep.c
    ../posix/thread.c
    ../posix/time.c
    ../posix/timeget.c
    ../all/panicf.c
    ../all/printf.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
    ${CMAKE_CURRENT_LIST_DIR}/../../../include/runtime-${RUNTIME}
)
