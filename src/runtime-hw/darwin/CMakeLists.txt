set(NAME "runtime-hw")

add_library(${NAME} STATIC
    ../stub/gpio.c
    ../stub/hw.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
