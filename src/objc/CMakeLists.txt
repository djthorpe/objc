set(NAME "fuse-objc")

add_library(${NAME}
    arm64/objc_msgSend.s
    autorelease.c
    debug.c
    runtime.c
    Object.m
)
