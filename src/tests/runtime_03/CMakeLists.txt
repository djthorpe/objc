set(NAME "runtime_03")
add_executable(${NAME}
    main.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
    malloc
)
add_test(NAME ${NAME} COMMAND ${NAME})
