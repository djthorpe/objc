set(NAME "runtime_02")

if(CMAKE_SYSTEM_NAME STREQUAL "PICO")
    add_library(${NAME} STATIC
        test.m
        TestClass.m
    )
else()
    add_executable(${NAME}
        main.c
        test.m
        TestClass.m
    )
    add_test(NAME ${NAME} COMMAND ${NAME})
endif()

target_link_libraries(${NAME}
    objc-gcc
    malloc # we use system-provided malloc for these tests
)
