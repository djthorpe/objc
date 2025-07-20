set(NAME "sys")

add_library(${NAME} STATIC
    abort.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
