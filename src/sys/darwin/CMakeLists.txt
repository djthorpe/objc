set(NAME "sys")

add_library(${NAME} STATIC
    malloc.c
    sleep.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
