set(NAME "runtime_04")

if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
    add_library(${NAME} STATIC
        test.m
        Test04.m
    )
else()
    add_executable(${NAME}
        main.c
        test.m
        Test04.m
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
endif()

target_link_libraries(${NAME}
    objc-gcc
    malloc # we use system-provided malloc for these tests
)
