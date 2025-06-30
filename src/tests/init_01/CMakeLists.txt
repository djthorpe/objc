set(NAME "init_01")
add_executable(${NAME}
    main.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
)
add_test(NAME ${NAME} COMMAND ${NAME})
