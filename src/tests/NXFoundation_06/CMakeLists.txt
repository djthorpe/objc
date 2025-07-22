set(NAME "NXFoundation_06")
add_executable(${NAME}
    main.m
)
target_link_libraries(${NAME} PRIVATE
    NXFoundation
)
# Ensure C11 standard is used for for-loop declarations
set_target_properties(${NAME} PROPERTIES
    C_STANDARD 11
    C_EXTENSIONS ON
)
add_test(NAME ${NAME} COMMAND ${NAME})
