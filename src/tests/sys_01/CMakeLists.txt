set(NAME "sys_01")
add_executable(${NAME}
    main.c
)
target_link_libraries(${NAME} PRIVATE
    sys
)
add_test(NAME ${NAME} COMMAND ${NAME})
