set(NAME "init_04")
add_executable(${NAME}
    main.m
    Test.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
)
add_test(NAME ${NAME} COMMAND ${NAME})
