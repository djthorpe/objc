set(NAME "runtime-sys")

add_library(${NAME} STATIC
    abort.c
    exception.c
    mutex.c
    random.c
    ../posix/malloc.c
    ../posix/puts.c
    ../posix/sleep.c
    ../posix/thread.c
    ../posix/time.c
    ../all/panicf.c
    ../all/printf.c
    ../all/time.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
