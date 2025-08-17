set(NAME "runtime-net")

add_library(${NAME} STATIC
  net.c
  ntp.c
)
target_include_directories(${NAME} PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/../../../include
  ${CMAKE_CURRENT_LIST_DIR}/../../../src/runtime-hw/pico
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

# Check if we're building for a board with CYW43 support
if (PICO_CYW43_SUPPORTED)
  message(STATUS "Board has CYW43 support - enabling CYW43 and lwIP libraries")
  target_compile_definitions(${NAME} PRIVATE PICO_CYW43_SUPPORTED=1)
  target_link_libraries(${NAME} 
    pico_cyw43_arch_lwip_poll 
    pico_lwip_sntp
  )
endif()
