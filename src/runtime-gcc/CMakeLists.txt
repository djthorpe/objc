set(NAME "objc-gcc")

add_library(${NAME} STATIC
    category.c
    class.c
    hash.c
    init.m
    message.c
    module.c
    NXConstantString.m
    Object.m
    Protocol.m
    protocol.c
    statics.c
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-gcc
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} PRIVATE
    sys
)

