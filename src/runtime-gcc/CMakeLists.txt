set(NAME "objc-runtime")

add_library(${NAME} STATIC
    class.c
    hash.c
    module.c
    Object.m
    NXConstantString.m
    panicf.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-gcc
)
