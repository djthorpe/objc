set(NAME "objc-gcc")

add_library(${NAME} STATIC
    category.c
    class.c
    hash.c
    hexdump.c
    module.c
    Object.m
    NXConstantString.m
    panicf.c
    statics.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-gcc
)
