set(NAME "objc")

add_library(${NAME}
    x86_64/objc_msgSend.s
    alloc.c
    autorelease.c
    debug.c
    runtime.c
    Object.m
)
