set(NAME "Network")

add_library(${NAME} STATIC
    NXNetworkTime.m
    NXWireless.m
    NXWirelessNetwork.m
)
target_include_directories(${NAME} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/../../include/runtime-${RUNTIME}
    ${CMAKE_CURRENT_LIST_DIR}/../../include
)
target_link_libraries(${NAME} PUBLIC
    runtime-hw
    runtime-net
    Foundation
)
