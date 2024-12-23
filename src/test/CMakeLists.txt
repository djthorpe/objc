set(NAME "fuse")

add_executable(${NAME}
    main.m
)

target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../include
)

target_link_libraries(${NAME} objc)
