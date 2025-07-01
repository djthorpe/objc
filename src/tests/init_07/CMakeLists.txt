set(NAME "init_07")
add_executable(${NAME}
    main.m
    Object+Description.m
)
target_link_libraries(${NAME} PRIVATE
    objc-gcc
)
add_test(NAME ${NAME} COMMAND ${NAME})
