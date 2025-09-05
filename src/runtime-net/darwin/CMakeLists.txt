set(NAME "runtime-net")

add_library(${NAME} STATIC
  ../stub/mqtt.c
  ../stub/net.c
  ../stub/ntp.c
)
target_include_directories(${NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../../../include
)
target_link_libraries(${NAME} 
  runtime-sys
)

# Always pull in net_poll from this archive when linked, so hooks are active
if (APPLE)
  # Mach-O requires leading underscore for -u
  target_link_options(${NAME} INTERFACE "-Wl,-u,_net_poll")
else()
  target_link_options(${NAME} INTERFACE "-Wl,-u,net_poll")
endif()
