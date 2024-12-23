set(NAME "objc")

add_library(${NAME}
    x86_64/objc_msgSend.s
    alloc.c
    autorelease.c
    runtime.c
    Object.m
)
