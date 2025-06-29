set(NAME "init_03")
add_executable(${NAME}
    main.m
)
target_link_libraries(${NAME} PRIVATE
    objc-runtime
)
add_test(NAME ${NAME} COMMAND ${NAME})
