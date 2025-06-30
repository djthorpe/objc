set(NAME "init_05")
add_executable(${NAME}
    main.m
    Test1.m
    Test2.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
)
add_test(NAME ${NAME} COMMAND ${NAME})
