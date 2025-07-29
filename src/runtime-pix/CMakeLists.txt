set(NAME "runtime-pix")

add_library(${NAME} STATIC
    pix.c
    rgba32.c
)
target_include_directories(${NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(${NAME} PRIVATE
    runtime-sys
)
