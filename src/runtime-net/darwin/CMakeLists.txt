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
