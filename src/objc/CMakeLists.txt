set(NAME "objc")

add_library(${NAME} STATIC
    misc.c
    objmsgsend-armv6m-pico.S
    objmsgsend.c
    Object.m
)
target_compile_options(${NAME} PRIVATE
    -fobjc-runtime=macosx
    -Wno-unused-command-line-argument
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
