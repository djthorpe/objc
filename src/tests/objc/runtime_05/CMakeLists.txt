set(NAME "runtime_05")

if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
    add_library(${NAME} STATIC
        test.m
        Test1.m
        Test2.m
    )
else()
    add_executable(${NAME}
        main.c
        test.m
        Test1.m
        Test2.m
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
endif()

target_link_libraries(${NAME}
    objc-gcc
    malloc # we use system-provided malloc for these tests
)
