set(NAME "objc-gcc")

add_library(${NAME} STATIC
    class.c
    init.c
    message.c
    method.c
    Object.m
    panicf.c
)
target_compile_options(${NAME} PRIVATE
    -fobjc-runtime=gcc
    -Wno-unused-command-line-argument
)
target_link_options(${NAME} PRIVATE
    -fobjc-runtime=gcc
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
