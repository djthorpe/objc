set(NAME "NXFoundation_13")
add_executable(${NAME}
    main.m
)
target_link_libraries(${NAME} PRIVATE
    NXFoundation
)
add_test(NAME ${NAME} COMMAND ${NAME})
