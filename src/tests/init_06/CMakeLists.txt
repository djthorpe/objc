set(NAME "init_06")
add_executable(${NAME}
    main.m
    Test.m
    Test+Description.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
)
add_test(NAME ${NAME} COMMAND ${NAME})
