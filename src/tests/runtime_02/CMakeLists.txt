set(NAME "runtime_02")
add_executable(${NAME}
    main.m
    Test.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
    malloc
)
add_test(NAME ${NAME} COMMAND ${NAME})
