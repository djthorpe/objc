set(NAME "sys_14")

if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
    add_library(${NAME} STATIC
        test.c
    )
else()
    add_executable(${NAME}
        main.c
        test.c
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
endif()

target_link_libraries(${NAME}
    runtime-sys
)
