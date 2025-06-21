set(NAME "test")

add_executable(${NAME}
    main.m
    Object.m
    Test.m
    misc-armv6m-pico.s
)

target_compile_options(${NAME} PRIVATE
    -fobjc-runtime=macosx
    -Wno-unused-command-line-argument
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
)
target_link_libraries(${NAME} pico_stdlib)