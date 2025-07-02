set(NAME "NXFoundation_01")
add_executable(${NAME}
    main.m
)
target_link_libraries(${NAME} PRIVATE
    NXFoundation
    malloc
)
add_test(NAME ${NAME} COMMAND ${NAME})
