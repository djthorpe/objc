set(NAME "sys")

add_library(${NAME} STATIC
    exception.c
    malloc.c
    mutex.c
    puts.c
    sleep.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
