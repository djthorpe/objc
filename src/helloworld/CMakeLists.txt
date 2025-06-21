set(NAME "helloworld")

set_link_options(-e _main)

add_executable(${NAME}
    main.s
)
