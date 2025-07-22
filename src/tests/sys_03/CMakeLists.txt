set(NAME "sys_03")
add_executable(${NAME}
    main.c
)
target_link_libraries(${NAME} PRIVATE
    runtime-sys
)
add_test(NAME ${NAME} COMMAND ${NAME})
