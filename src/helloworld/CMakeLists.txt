set(NAME "helloworld")
set(CMAKE_EXE_LINKER_FLAGS "-e _start")

add_executable(${NAME}
    main.s
)
